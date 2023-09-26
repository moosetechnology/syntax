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

/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* Actions semantiques construisant la f_structure d'une analyse LFG */
/* L'analyseur est de type Earley */
/* Le source est sous forme de DAG */

static char	ME [] = "lfg_semact";

/* Evaluation d'une f_structure sur un arbre */
/* On suppose que les f_structures de toutes les feuilles lexicales ont ete remplies,
   Elles seront partagees entre toutes les analyses */
/* Soit forest_node_nb le nombre de noeuds de la foret partagee et
   forest_prod_nb son nb de prod instanciees */
/* L'evaluation va se faire en plusieurs passes */

#include "sxunix.h"
#include "earley.h"
char WHAT_LFGSEMACT[] = "@(#)SYNTAX - $Id: lfg_semact.c 3491 2023-08-20 14:56:17Z garavel $" WHAT_DEBUG;

#if 0
/* A voir + tard... */
#include "lfg.h"
#endif /* 0 */
SXBOOLEAN is_print_f_structure, is_unfold_parse_forest, is_forest_filtering, is_sum_priority;
SXINT     sentence_id, rank [6];
FILE    *xml_file;
char    *xml_file_name, *input_sentence_string, *input_dag_string;

#include "dag_scanner.h"
#include "XH.h"
#include lfg_h
#include lex_lfg_h


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
#define X7F             ((~((SXUINT) 0))>>1)
#define X80             (~X7F)
#define X40             (X80>>1)
#define X3F             ((~(0u))>>2)
#define XC0             (~(X3F)) 

#define CONSTRAINT_VAL  (ASSIGN_CONSTRAINT_KIND<<MAX_FIELD_ID_SHIFT)

#define FS_ID_SHIFT     (STRUCTURE_NAME_shift+MAX_FIELD_ID_SHIFT)

#if ATOM_Aij
static SXINT              AIJ;
#endif /* ATOM_Aij */
#if ATOM_Pij
static SXINT              PIJ;
#endif /* ATOM_Pij */

#define FIELD_AND       (~((~0)<<MAX_FIELD_ID_SHIFT))
#define KIND_AND        (~((~0)<<STRUCTURE_NAME_shift))
#define FS_ID_AND        (~((~0)<<FS_ID_SHIFT))

#include "sxstack.h"

#define SXBA_fdcl(n,l)	SXBA_ELT n [SXNBLONGS (l) + 1] = {l,}

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

static SXINT               maxxt; /* nb des terminaux instancies Tij, y compris ceux couvrant les nt de la rcvr
				   (maxxt >= -spf.outputG.maxt) */
static SXUINT tree_id;
static SXINT               Xpq_top;
static XxY_header        heads;
static XH_header         XH_fs_hd, XH_ufs_hd;
static SXBA              fs_id_set; /* pour print_f_structure () */
static SXBA              ufs_id_set; /* pour uwfs2ufs_id () */
static SXINT               *fs_id_stack;
static SXINT               rhs_pos2fs [MAXRHS+1];
static SXINT               rhs_pos2digit [MAXRHS+1];
#if 0
static SXINT               shared_f_struct2wfs_id [MAX_SHARED_F_STRUCTURE_ID+1];
static SXBA_ELT          shared_f_struct_id_set [SXNBLONGS (MAX_SHARED_F_STRUCTURE_ID+1)+1] = {MAX_SHARED_F_STRUCTURE_ID+1};
static SXBA              cyclic_wfs_id_set;
static SXINT               *wfs_id2simulated_son_nb, *simulated_wfs_id_stack, *cyclic_wfs_id2simulated_fs_id;
#endif /* 0 */

/* Pour un Xpq donne', soit ptr = fs_id_dstack+Xpq2disp[Xpq], on a :
   - *ptr == nb de fs differentes associees a Xpq
   - pour tout i, 1 <= i <= *ptr, ptr [i] est un fs_id */
static SXINT               *Xpq2disp, *fs_id_dstack;
static SXBA              Xpq_set; /* Cas unfold pour un clear apres chaque arbre */

/* Pour traiter le $$ courant */
static SXINT               wfs_id, wfs_page_nb;
static SXINT               *wfs_id2son_nb, *wfs_id_stack;
static SXINT               *wfs_id2equiv;


static struct pred_list_hd {
  SXINT first, last;
} *wfs_id2wfs_id_pred_list;

static struct wfs_id_pred_item {
  SXINT wfs_id, *field_ptr, next;
} *wfs_id_pred_list;

static SXINT    shared_struct_nb; /* local a chaque $$ */

struct shared_rhs {
  SXINT rhs [MAXRHS+1];
}; 

static struct shared_rhs *shared_fs_id2wfs_id;
static struct shared_rhs empty_shared_rhs;

static SXINT               *shared_fs_id_stack;
static SXBA              wfs_id_set;
static SXBA              **wfs_field_set;
static SXINT               ***wfs2attr; /* (val<<STRUCTURE_NAME_shift)+kind */

static SXBA_fdcl (doli_used_set, MAXRHS+1);
/* Fin pour traiter le $$ courant */

static SXINT *post_stack, *post_display; /* DSTACKs */

#define POST_STACK_fs_id_x -1
#define POST_STACK_x_y     -2


static SXBA_fdcl (warg_set, MAX_FIELD_ID+1);
static SXBA_fdcl (pred_arg_set, MAX_FIELD_ID+1);

static SXINT               Aij; /* La lhs de la prod courante ($$) */
static struct rhs        *prhs; /* on a Xpq = prhs [i].lispro  et $i = |Xpq| */
static SXINT               *pcs; /* pointeur courant ds code_stack */

static XH_header         dynam_pred_vals; /* Quadruplets : lexeme_id, sous_cat1, sous_cat2, priority */

static SXBA              failure_set;


static struct re_result {
  SXINT *ptr, kind, next;
}                        *re_results;
static SXINT               *re_tank;


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

static XxY_header prio_hd, dl_hd;
static SXINT        *dl_id2fs_id;
static SXBA_ELT   index_set [SXNBLONGS (MAX_SHARED_F_STRUCTURE_ID+1)+1] = {MAX_SHARED_F_STRUCTURE_ID+1};
static SXINT        index2struct_id [MAX_SHARED_F_STRUCTURE_ID+1];

static SXBOOLEAN    is_consistent; /* !is_consistent <=> toutes les f_struct sont incoherentes */
static SXBOOLEAN    is_rcvr_done; /* SXTRUE <=> Aucune f_structure a la racine, on a recupere ce qu'on a pu ... */
static SXBA       head_set;
static SXINT        *head_stack;
static SXINT        ranking_kind;

/* Pour la passe "is_forest_filtering" */
static SXINT     *Pij2disp, *fs_id_Pij_dstack, Pij_cur_bot;
static SXINT     doldol;
static SXBA    unconsistent_heads_set;
static SXBOOLEAN is_locally_unconsistent;

/* Pour dag_td_walk */
static SXBA    valid_heads_set;

static SXBOOLEAN is_relaxed_run; /* positionne si le 1er passage a echoue'.  On en refait un sans la verif de la coherence */

static SXINT              *Tij2tok_no;

static void
dl_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  dl_id2fs_id = (SXINT*) sxrealloc (dl_id2fs_id, new_size+1, sizeof (SXINT));
}

static void
wfs_area_finalize ()
{
  SXINT             x, *q, **p;

  wfs_field_set [wfs_page_nb-1] = sxbm_calloc (AREA_SIZE, MAX_FIELD_ID+1);
  p = wfs2attr [wfs_page_nb-1] = (SXINT**) sxalloc (AREA_SIZE, sizeof (SXINT*));
  q = p [0] = (SXINT*) sxalloc (AREA_SIZE*(MAX_FIELD_ID+1), sizeof (SXINT));

  for (x = 1; x < AREA_SIZE; x++)
    p [x] = (q += MAX_FIELD_ID+1);
}

static void
wfs_area_allocate ()
{
  SXINT struct_id;

  wfs_page_nb = 1;
  wfs_field_set = (SXBA **) sxalloc (wfs_page_nb, sizeof (SXBA*));
  wfs2attr = (SXINT***) sxalloc (wfs_page_nb, sizeof (SXINT**));

  wfs_id2son_nb = (SXINT*) sxalloc (AREA_SIZE, sizeof (SXINT)), wfs_id2son_nb [0] = 0;
  wfs_id_stack = (SXINT*) sxalloc (AREA_SIZE+1, sizeof (SXINT)), wfs_id_stack [0] = 0;
  
  wfs_id_set = sxba_calloc (AREA_SIZE);
#if 0
  cyclic_wfs_id_set = sxba_calloc (AREA_SIZE);
  cyclic_wfs_id2simulated_fs_id = (SXINT*) sxalloc (AREA_SIZE, sizeof (SXINT));
  wfs_id2simulated_son_nb = (SXINT*) sxalloc (AREA_SIZE, sizeof (SXINT));
  simulated_wfs_id_stack = (SXINT*) sxalloc (AREA_SIZE+1, sizeof (SXINT)), simulated_wfs_id_stack [0] = 0;
#endif /* 0 */

  wfs_id2equiv = (SXINT*) sxalloc (AREA_SIZE, sizeof (SXINT));
  wfs_id2wfs_id_pred_list = (struct pred_list_hd*) sxalloc (AREA_SIZE, sizeof (struct pred_list_hd));
  wfs_id_pred_list = (struct wfs_id_pred_item*) sxalloc (AREA_SIZE+1, sizeof (struct wfs_id_pred_item));
  wfs_id_pred_list [0].wfs_id = 0; /* top */
  wfs_id_pred_list [0].next = AREA_SIZE; /* size */

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
  SXINT l;

  wfs_page_nb++;
  wfs_field_set = (SXBA **) sxrealloc (wfs_field_set, wfs_page_nb, sizeof (SXBA*));
  wfs2attr = (SXINT***) sxrealloc (wfs2attr, wfs_page_nb, sizeof (SXINT**));

  l = AREA_SIZE*wfs_page_nb;

  wfs_id2son_nb = (SXINT*) sxrealloc (wfs_id2son_nb, l, sizeof (SXINT));
  wfs_id_stack = (SXINT*) sxrealloc (wfs_id_stack, l+1, sizeof (SXINT));
  wfs_id_set = sxba_resize (wfs_id_set, l);
#if 0
  cyclic_wfs_id_set = sxba_resize (cyclic_wfs_id_set, l);
  cyclic_wfs_id2simulated_fs_id = (SXINT*) sxrealloc (cyclic_wfs_id2simulated_fs_id, l+1, sizeof (SXINT));
  wfs_id2simulated_son_nb = (SXINT*) sxrealloc (wfs_id2simulated_son_nb, l, sizeof (SXINT));
  simulated_wfs_id_stack = (SXINT*) sxrealloc (simulated_wfs_id_stack, l+1, sizeof (SXINT));
#endif /* 0 */
  wfs_id2equiv = (SXINT*) sxrealloc (wfs_id2equiv, l, sizeof (SXINT));
  wfs_id2wfs_id_pred_list = (struct pred_list_hd*) sxrealloc (wfs_id2wfs_id_pred_list, l, sizeof (struct pred_list_hd));

  wfs_area_finalize ();
}

static void
wfs_area_free ()
{
  SXINT x, struct_id, **p;
  static void free_store_wfs ();

  for (x = wfs_page_nb-1; x >= 0; x--) {
    sxbm_free (wfs_field_set [x]);
    p =  wfs2attr [x];
    sxfree (*p);
    sxfree (p);
  }

  sxfree (wfs_field_set), wfs_field_set = NULL;
  sxfree (wfs2attr), wfs2attr = NULL;

  sxfree (wfs_id2son_nb), wfs_id2son_nb = NULL;
  sxfree (wfs_id_stack), wfs_id_stack = NULL;
  sxfree (wfs_id_set), wfs_id_set = NULL;
#if 0
  sxfree (cyclic_wfs_id_set), cyclic_wfs_id_set = NULL;
  sxfree (cyclic_wfs_id2simulated_fs_id), cyclic_wfs_id2simulated_fs_id = NULL;
  sxfree (wfs_id2simulated_son_nb), wfs_id2simulated_son_nb = NULL;
  sxfree (simulated_wfs_id_stack), simulated_wfs_id_stack = NULL;
#endif /* 0 */
  sxfree (wfs_id2equiv), wfs_id2equiv = NULL;
  sxfree (wfs_id2wfs_id_pred_list), wfs_id2wfs_id_pred_list = NULL;
  sxfree (wfs_id_pred_list), wfs_id_pred_list = NULL;

#if EBUG
  varstr_free (left_vstr), left_vstr = NULL;
  varstr_free (right_vstr), right_vstr = NULL;
#endif /* EBUG */

  varstr_free (vstr), vstr = NULL;

  free_store_wfs ();
}

static SXINT
get_a_new_wfs_id ()
{
  ++wfs_id;
		
  if (AREA_PAGE (wfs_id) >= wfs_page_nb)
    wfs_area_oflw ();
    
  wfs_id2son_nb [wfs_id] = 0;
  wfs_id2equiv [wfs_id] = wfs_id;
  wfs_id2wfs_id_pred_list [wfs_id].first = wfs_id2wfs_id_pred_list [wfs_id].last = 0;

  return wfs_id;
}

static SXINT
get_new_top_pred_wfs_id ()
{
  SXINT top;

  if ((top = ++wfs_id_pred_list [0].wfs_id) > wfs_id_pred_list [0].next)
    wfs_id_pred_list = (struct wfs_id_pred_item*) sxrealloc (wfs_id_pred_list,
							     (wfs_id_pred_list [0].next *= 2)+1,
							     sizeof (struct wfs_id_pred_item));

  return top;
}


/* On ajoute en fin de liste le couple (pred_id, field_ptr) ds les predecesseurs du wfs_id id */
/* ... et on en profite pour dire que le nb de "fils" de pred_id a augmente' de 1 */
static void
put_new_pred_wfs_id (id, pred_id, field_ptr)
     SXINT id, pred_id, *field_ptr;
{
  SXINT                     top, last;
  struct wfs_id_pred_item *pred_ptr;
  struct pred_list_hd     *hd_ptr;

  top = get_new_top_pred_wfs_id ();
  pred_ptr = wfs_id_pred_list + top;
  pred_ptr->wfs_id = pred_id;
  pred_ptr->field_ptr = field_ptr;
  pred_ptr->next = 0;

  hd_ptr = wfs_id2wfs_id_pred_list + id;

  if ((last = hd_ptr->last) == 0)
    hd_ptr->first = top;
  else
    wfs_id_pred_list [last].next = top;

  hd_ptr->last = top;

  if (pred_id < 0)
    pred_id = -pred_id;

  wfs_id2son_nb [pred_id]++;
}

static SXINT
get_equiv (id)
     SXINT id;
{
  SXINT main_id;

  while ((main_id = wfs_id2equiv [id]) != id)
    id = main_id;

  return id;
}

/* les wfs_id id1 et id2 fusionnent ds id1 ... */
/* On suppose que l'unification a deja ete faite (et a reussie) ds id1 */
static void
merge (id1, id2)
     SXINT id1, id2;
{
  SXINT                     last1, first2, *field_ptr, *field_ptr2;
  struct wfs_id_pred_item *pred_ptr2;
  struct pred_list_hd     *hd_ptr1, *hd_ptr2;

  id1 = get_equiv (id1);
  id2 = get_equiv (id2);

  if (id1 == id2) return; /* Prudence */

  /* id1 et id2 ne sont pas ds la meme classe d'equivalence */
  wfs_id2equiv [id2] = id1; /* Ils le sont ... */

  hd_ptr2 = wfs_id2wfs_id_pred_list + id2;

  if (first2 = hd_ptr2->first) {
    do {
      pred_ptr2 = wfs_id_pred_list + first2;
      field_ptr2 = pred_ptr2->field_ptr;
      /* *field_ptr = id2||kind||field_id */
      /* on le remplace par id1||kind||field_id */
      *field_ptr2 = (id1<<FS_ID_SHIFT) + (*field_ptr2&FS_ID_AND);
    } while (first2 = pred_ptr2->next);

    /* Concatenation des 2 listes */
    hd_ptr1 = wfs_id2wfs_id_pred_list + id1;

    if (last1 = hd_ptr1->last)
      wfs_id_pred_list [last1].next = hd_ptr2->first;
    else
      /* La liste id1 est vide !! */
      hd_ptr1->first = hd_ptr2->first;

    hd_ptr1->last = hd_ptr2->last;
    /* prudence !! */
    hd_ptr2->first = hd_ptr1->first;
    wfs_id2son_nb [id2] = 0;
    sxba_empty (wfs_field_set [AREA_PAGE (id2)] [AREA_DELTA (id2)]);
  }
}

static void
heads_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  unconsistent_heads_set = sxba_resize (unconsistent_heads_set, new_size+1);
}

static void
XH_fs_hd_oflw (old_size, new_size)
     SXINT old_size, new_size;
{
  fs_id_set = sxba_resize (fs_id_set, new_size+1);
  ufs_id_set = sxba_resize (ufs_id_set, new_size+1);
  fs_id_stack = (SXINT*) sxrealloc (fs_id_stack, new_size+1, sizeof (SXINT));
  shared_fs_id2wfs_id = (struct shared_rhs*) sxrealloc (shared_fs_id2wfs_id, new_size+1, sizeof (struct shared_rhs));
}


static SXINT
re_results_get_new_top ()
{
  SXINT top;

  if ((top = ++re_results [0].kind) > re_results [0].next)
    re_results = (struct re_result*) sxrealloc (re_results, (re_results [0].next *= 2)+1, sizeof (struct re_result));

  return top;
}

static void
allocate_all ()
{
  XH_alloc (&dynam_pred_vals, "dynam_pred_vals", 2*n+1, 1, 4, NULL, NULL);

  Xpq2disp = (SXINT*) sxcalloc (Xpq_top+1, sizeof (SXINT));
  Xpq_set = sxba_calloc (Xpq_top+1);
  DALLOC_STACK (fs_id_dstack, 3*Xpq_top);

  XxY_alloc (&heads, "heads", 2*Xpq_top+1, 1, 0 /* pas de foreach */, 0, heads_oflw /* pour unconsistent_heads_set */, NULL);
  unconsistent_heads_set = sxba_calloc (XxY_size (heads)+1);

  /* Liste (ordonnee) de couples (field, val) */
  XH_alloc (&XH_fs_hd, "XH_fs_hd", Xpq_top+1, 1, 2*5, XH_fs_hd_oflw, NULL); /* A VOIR */
  fs_id_set = sxba_calloc (XH_size (XH_fs_hd)+1);
  ufs_id_set = sxba_calloc (XH_size (XH_fs_hd)+1);
  fs_id_stack = (SXINT*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (SXINT)), fs_id_stack [0] = 0;

  XH_alloc (&XH_ufs_hd, "XH_fs_hd", 20+1, 1, 5, NULL, NULL); /* A VOIR */

  shared_fs_id2wfs_id = (struct shared_rhs*) sxalloc (XH_size (XH_fs_hd)+1, sizeof (struct shared_rhs));
  DALLOC_STACK (shared_fs_id_stack, AREA_SIZE);

  /* On alloue les f_structures de travail ... */
  wfs_area_allocate ();

  re_results = (struct re_result*) sxalloc (25+1, sizeof (struct re_result));
  re_results [0].kind = 0; /* top */
  re_results [0].next = 25; /* size */
  DALLOC_STACK (re_tank, 10);

  XxY_alloc (&prio_hd, "prio_hd", n+1, 1, 0 /* pas de foreach */, 0, NULL, NULL);
  XxY_alloc (&dl_hd, "dl_hd", 2*n+1, 1, 0 /* pas de foreach */, 0, dl_hd_oflw, NULL);
  dl_id2fs_id = (SXINT*) sxalloc (XxY_size (dl_hd)+1, sizeof (SXINT));

  DALLOC_STACK (post_stack, 100);
  DALLOC_STACK (post_display, 10);

  if (is_forest_filtering || xml_file) {
    Pij2disp = (SXINT*) sxcalloc (spf.outputG.maxprod+1, sizeof (SXINT));
    DALLOC_STACK (fs_id_Pij_dstack, 5*spf.outputG.maxprod+1);
  } 

  Tij2tok_no = (SXINT*) sxcalloc (maxxt+1, sizeof (SXINT));
}

static void
free_all ()
{
  XH_free (&dynam_pred_vals);

  sxfree (Xpq2disp), Xpq2disp = NULL;
  sxfree (Xpq_set), Xpq_set = NULL;
  DFREE_STACK (fs_id_dstack);
  XxY_free (&heads);
  sxfree (unconsistent_heads_set), unconsistent_heads_set = NULL;
  XH_free (&XH_fs_hd);
  sxfree (fs_id_set), fs_id_set = NULL;
  sxfree (fs_id_stack), fs_id_stack = NULL;
  if (head_stack) sxfree (head_stack), head_stack = NULL;
  if (head_set) sxfree (head_set), head_set = NULL;
  sxfree (shared_fs_id2wfs_id), shared_fs_id2wfs_id = NULL;
  DFREE_STACK (shared_fs_id_stack);
  XH_free (&XH_ufs_hd);
  sxfree (ufs_id_set), ufs_id_set = NULL;
  wfs_area_free ();
  sxfree (re_results), re_results = NULL;
  DFREE_STACK (re_tank);

  if (failure_set) sxfree (failure_set), failure_set = NULL;

  XxY_free (&prio_hd);
  XxY_free (&dl_hd);
  sxfree (dl_id2fs_id), dl_id2fs_id = NULL;

  DFREE_STACK (post_stack);
  DFREE_STACK (post_display);

  if (is_forest_filtering || xml_file) {
    sxfree (Pij2disp), Pij2disp = NULL;
    DFREE_STACK (fs_id_Pij_dstack);
  }
    
  sxfree (Tij2tok_no), Tij2tok_no = NULL;
}


/* L'unification a echoue, remise en etat de la structure de travail */
static void
clear_wfs ()
{
  SXINT x, page_nb, delta_nb, id;
  SXBA field_set;

  for (x = 0; x <= wfs_id; x++) {
    page_nb = AREA_PAGE (x);
    delta_nb = AREA_DELTA (x);
    sxba_empty (wfs_field_set [page_nb] [delta_nb]);
    wfs_id2son_nb [x] = 0;
  }
}


/* Recopie l'ensemble de structures top ds XH_ufs_id */
static SXINT
uwfs2ufs_id (top, store_ste)
     SXINT top;
     SXBOOLEAN store_ste;
{
  SXINT     id, ufs_id;
  SXBOOLEAN is_empty;

  if (top <= 0)
    return 0;
  
#if ATOM_Aij || ATOM_Pij
  if (store_ste) {
    /* wfs_id_pred_list contient des Aij Pij */
    do {
      id = wfs_id_pred_list [top].wfs_id;
      XH_push (XH_ufs_hd, id);
    } while ((top = wfs_id_pred_list [top].next) != 0);

    XH_set (&XH_ufs_hd, &ufs_id);

    return ufs_id;
  }
#endif /* ATOM_Aij || ATOM_Pij */

  is_empty = SXTRUE;

  do {
    id = wfs_id_pred_list [top].wfs_id;

    if (id != -1 ) {
      if (id & X80) {
	id &= X7F;
	id = wfs2attr [AREA_PAGE (id)] [AREA_DELTA (id)] [0];
	    
	/* Si id < 0 => on pointe vers une structure partagee */
	if (id < 0)
	  id = -id;
      }
	
      SXBA_1_bit (ufs_id_set, id);
      is_empty = SXFALSE;
    }

  } while ((top = wfs_id_pred_list [top].next) != 0);

  if (is_empty)
    return 0;

  id = 0;

  while ((id = sxba_scan_reset (ufs_id_set, id)) > 0) {
    XH_push (XH_ufs_hd, id);
  }
  
  XH_set (&XH_ufs_hd, &ufs_id);

  return ufs_id;
}


struct store_wfs {
  SXINT                     wfs_id, wfs_page_nb, XH_fs_hd_size;
  SXBA                    **wfs_field_set;
  SXINT                     ***wfs2attr; /* (val<<STRUCTURE_NAME_shift)+kind */
  SXINT                     *wfs_id2son_nb;
  SXINT                     *wfs_id2equiv;
  struct pred_list_hd     *wfs_id2wfs_id_pred_list;
  struct wfs_id_pred_item *wfs_id_pred_list;
  struct shared_rhs       *shared_fs_id2wfs_id; 
  SXINT                     *shared_fs_id_stack;
  SXBA                    wfs_id_set;
  SXBA                    doli_used_set;
};

static struct store_wfs_hd {
  SXINT              wfs_size;
  struct store_wfs **wfs;
  SXBA             wfs_set;
} store_wfs_hd;

/* Procedure utilisee ds le cas d'expressions regulieres non triviales sur $$ */
/* Elle stocke la valeur actuelle de wfs */
static SXINT
put_in_store_wfs ()
{
  SXINT              i, size;
  SXINT              cur_wfs_id, page_nb, delta_nb, id, fs_id, field_id;
  SXINT              *attr_ptr, *store_attr_ptr;
  SXINT              x, *q, **p;
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

    /* ... et on alloue */
    size = AREA_SIZE*wfs_page_nb;

    store_wfs_ptr->wfs_field_set = (SXBA **) sxalloc (wfs_page_nb, sizeof (SXBA*));
    store_wfs_ptr->wfs2attr = (SXINT***) sxalloc (wfs_page_nb, sizeof (SXINT**));
    store_wfs_ptr->wfs_id2son_nb = (SXINT*) sxalloc (size, sizeof (SXINT));
    store_wfs_ptr->wfs_id2equiv = (SXINT*) sxalloc (size, sizeof (SXINT));
    store_wfs_ptr->wfs_id2wfs_id_pred_list = (struct pred_list_hd*) sxalloc (size, sizeof (struct pred_list_hd));
    store_wfs_ptr->wfs_id_pred_list = (struct wfs_id_pred_item*) sxalloc (size+1, sizeof (struct wfs_id_pred_item));
    /* ATTENTION : la reutilisation peut necessiter une reallocation */
    store_wfs_ptr->shared_fs_id2wfs_id = (struct shared_rhs*) sxalloc ((store_wfs_ptr->XH_fs_hd_size = XH_size (XH_fs_hd))+1,
								       sizeof (struct shared_rhs));
    DALLOC_STACK (store_wfs_ptr->shared_fs_id_stack, shared_fs_id_stack [-1]);
    store_wfs_ptr->wfs_id_set = sxba_calloc (size);
    store_wfs_ptr->doli_used_set = sxba_calloc (MAXRHS+1);

    for (page_nb = 0; page_nb < wfs_page_nb; page_nb++) {
      store_wfs_ptr->wfs_field_set [page_nb] = sxbm_calloc (AREA_SIZE, MAX_FIELD_ID+1);
      p = store_wfs_ptr->wfs2attr [page_nb] = (SXINT**) sxalloc (AREA_SIZE, sizeof (SXINT*));
      q = p [0] = (SXINT*) sxalloc (AREA_SIZE*(MAX_FIELD_ID+1), sizeof (SXINT));

      for (x = 1; x < AREA_SIZE; x++)
	p [x] = (q += MAX_FIELD_ID+1);
    }
  }

  if (store_wfs_ptr->XH_fs_hd_size < XH_size (XH_fs_hd)) {
    store_wfs_ptr->shared_fs_id2wfs_id = (struct shared_rhs*) sxrealloc (store_wfs_ptr->shared_fs_id2wfs_id,
							   (store_wfs_ptr->XH_fs_hd_size = XH_size (XH_fs_hd))+1,
							   sizeof (struct shared_rhs));
  }

  DRAZ (store_wfs_ptr->shared_fs_id_stack);
  DCHECK (store_wfs_ptr->shared_fs_id_stack, DTOP (shared_fs_id_stack));

  /* ici la structure store_wfs_ptr est prete a accueillir la copie (elle est allouee et sa taille est suffisante) */
  store_wfs_ptr->wfs_page_nb = wfs_page_nb;
  store_wfs_ptr->wfs_id = wfs_id;

  for (id = 0; id <= DTOP (shared_fs_id_stack); id++) {
    store_wfs_ptr->shared_fs_id_stack [id] = fs_id = shared_fs_id_stack [id];

    if (id > 0)
      /* sinon fs_id n'est pas un fs_id */
      store_wfs_ptr->shared_fs_id2wfs_id [fs_id] = shared_fs_id2wfs_id [fs_id];
  }

  sxba_copy (store_wfs_ptr->wfs_id_set, wfs_id_set);
  sxba_copy (store_wfs_ptr->doli_used_set, doli_used_set);

  for (cur_wfs_id = 0; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    store_wfs_ptr->wfs_id2equiv [cur_wfs_id] = wfs_id2equiv [cur_wfs_id];
    store_wfs_ptr->wfs_id2son_nb [cur_wfs_id] = wfs_id2son_nb [cur_wfs_id];
    store_wfs_ptr->wfs_id2wfs_id_pred_list [cur_wfs_id] = wfs_id2wfs_id_pred_list [cur_wfs_id];
    store_wfs_ptr->wfs_id_pred_list [cur_wfs_id] = wfs_id_pred_list [cur_wfs_id];
    sxba_copy (store_wfs_ptr->wfs_field_set [page_nb] [delta_nb], field_set = wfs_field_set [page_nb] [delta_nb]);
      
    attr_ptr = wfs2attr [page_nb] [delta_nb];
    store_attr_ptr = store_wfs_ptr->wfs2attr [page_nb] [delta_nb];

    field_id = 0;

    while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
      store_attr_ptr [field_id] = attr_ptr [field_id];
    }
  }

  return i;
}

static void
free_store_wfs ()
{
  SXINT              i, x, **p;
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

	sxfree (store_wfs_ptr->wfs_id2son_nb);
	sxfree (store_wfs_ptr->wfs_id2equiv);
	sxfree (store_wfs_ptr->wfs_id2wfs_id_pred_list);
	sxfree (store_wfs_ptr->wfs_id_pred_list);
	sxfree (store_wfs_ptr->shared_fs_id2wfs_id);
	DFREE_STACK (store_wfs_ptr->shared_fs_id_stack);
	sxfree (store_wfs_ptr->wfs_id_set);
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
     SXINT i;
{
  SXINT              cur_wfs_id, page_nb, delta_nb, id, fs_id, field_id, pos;
  SXINT              *attr_ptr, *store_attr_ptr;
  SXBA             field_set;
  struct store_wfs *store_wfs_ptr;

  store_wfs_ptr = store_wfs_hd.wfs [i];

  wfs_page_nb = store_wfs_ptr->wfs_page_nb;
  wfs_id = store_wfs_ptr->wfs_id;

  sxba_copy (wfs_id_set, store_wfs_ptr->wfs_id_set);
  sxba_copy (doli_used_set, store_wfs_ptr->doli_used_set);

  /* Attention, il faut d'abord razer completement shared_fs_id2wfs_id ... */
  while (DTOP (shared_fs_id_stack)) {
    pos = DPOP (shared_fs_id_stack);
    shared_fs_id2wfs_id [DPOP (shared_fs_id_stack)].rhs [pos] = -1;
  }

  for (id = 0; id <= DTOP (store_wfs_ptr->shared_fs_id_stack); id++) {
    shared_fs_id_stack [id] = fs_id = store_wfs_ptr->shared_fs_id_stack [id];

    if (id > 0)
      /* sinon fs_id n'est pas un fs_id */
      shared_fs_id2wfs_id [fs_id] = store_wfs_ptr->shared_fs_id2wfs_id [fs_id];
  }

  for (cur_wfs_id = 0; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    page_nb = AREA_PAGE (cur_wfs_id);
    delta_nb = AREA_DELTA (cur_wfs_id);
    wfs_id2equiv [cur_wfs_id] = store_wfs_ptr->wfs_id2equiv [cur_wfs_id];
    wfs_id2son_nb [cur_wfs_id] = store_wfs_ptr->wfs_id2son_nb [cur_wfs_id];
    wfs_id2wfs_id_pred_list [cur_wfs_id] = store_wfs_ptr->wfs_id2wfs_id_pred_list [cur_wfs_id];
    wfs_id_pred_list [cur_wfs_id] = store_wfs_ptr->wfs_id_pred_list [cur_wfs_id];
    sxba_copy (wfs_field_set [page_nb] [delta_nb], field_set = store_wfs_ptr->wfs_field_set [page_nb] [delta_nb]);
      
    attr_ptr = wfs2attr [page_nb] [delta_nb];
    store_attr_ptr = store_wfs_ptr->wfs2attr [page_nb] [delta_nb];

    field_id = 0;

    while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
      attr_ptr [field_id] = store_attr_ptr [field_id];
    }
  }
}


static void
close_store_wfs_level (i)
     SXINT i;
{
  SXBA_0_bit (store_wfs_hd.wfs_set, i); /* le niveau i peut etre reutilise' */
}

/* On travaille ds fs */
static SXBOOLEAN
fs_is_set (fs_id, field_id, ret_val)
     SXINT fs_id, field_id, **ret_val;
{
  /* Recherche dichotomique */
  SXINT bot, top, cur, cur_val, cur_field_id, *cur_val_ptr;

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
coherence (Xpq)
     SXINT     Xpq;
{
  SXINT                    cur_wfs_id, page_nb, delta_nb, i, sous_cat_id, cur2, top2, cur3, bot3, top3, field_id, nb;
  SXINT                    *attr_ptr0;
  SXBA                   field_set;
  SXBOOLEAN                is_root, is_optional, ret_val;
  SXINT                    *dpv_ptr, *cur_val_ptr;
  SXINT                    val, fs_id, top, bot, cur, ufs_id;
  unsigned char          static_field_kind;

  is_locally_unconsistent = SXFALSE;

  if (is_relaxed_run)
    return SXTRUE;

  is_root = Xpq == spf.outputG.start_symbol;
  ret_val = SXTRUE;

  for (cur_wfs_id = 0; cur_wfs_id <= wfs_id; cur_wfs_id++) {
    if (cur_wfs_id == wfs_id2equiv [cur_wfs_id]) {
      page_nb = AREA_PAGE (cur_wfs_id);
      delta_nb = AREA_DELTA (cur_wfs_id);

      field_set = wfs_field_set [page_nb] [delta_nb];
      attr_ptr0 = wfs2attr [page_nb] [delta_nb];

      if (SXBA_bit_is_set (field_set, PRED_ID)) {
	sxba_empty (pred_arg_set);
	dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, (attr_ptr0 [PRED_ID])>>FS_ID_SHIFT)));

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

		if (is_root && (nb > 1 || nb == 0 && !is_optional)) {
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

		if (is_root && !is_optional) {
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
	  ret_val = SXFALSE;
	  break;
	}
      }
      else {
	if (is_root) {
	  /* Il doit y avoir un pred */
	  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	  is_locally_unconsistent = SXTRUE;
	}
      }

      if (is_root) {
	/* On verifie qu'il ne reste pas de contrainte non verifiee */
	field_id = PRED_ID;

	while ((field_id = sxba_scan (field_set, field_id)) >= 0) {
	  if ((val = attr_ptr0 [field_id]) & CONSTRAINT_VAL) {
	  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
	    is_locally_unconsistent = SXTRUE;
	  }
	  else {
	    /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
	       ulterieure */
	    if (field_id2kind [field_id] == STRUCT_KIND+UNBOUNDED_KIND) {
	      val >>= FS_ID_SHIFT;

	      if (val > 0) {
		do {
		  fs_id = wfs_id_pred_list [val].wfs_id;

		  if (fs_id != -1 ) {
		    if ((fs_id & X80) == 0) {
		      /* C'est un (vrai) fs_id */
		      if (SXBA_bit_is_reset_set (fs_id_set, fs_id))
			PUSH (fs_id_stack, fs_id);
		    }
		  }
		} while ((val = wfs_id_pred_list [val].next) != 0);
	      }
	    }
	  }
	}
      }
    }
  }

  if (is_root) {
    if (ret_val && !is_locally_unconsistent) {
      if (fs_id_stack [0]) {
	while (!IS_EMPTY (fs_id_stack) && !is_locally_unconsistent) {
	  fs_id = POP (fs_id_stack);
	  bot = cur = XH_X (XH_fs_hd, fs_id);
	  top = XH_X (XH_fs_hd, fs_id+1);
	
	  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
	    /* shared */
	    top--;

	  if (bot == top) {
	    /* vide et donc pas de PRED */
	    is_locally_unconsistent = SXTRUE;
	    break;
	  }

	  while (cur < top && !is_locally_unconsistent) {
	    val = XH_list_elem (XH_fs_hd, cur);
	    field_id = val & FIELD_AND;

	    if (cur == bot) {
	      if (field_id == PRED_ID) {
		dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val>>FS_ID_SHIFT)));

		for (i = 1; i <= 2 && !is_locally_unconsistent; i++) {
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

			  if (SXBA_bit_is_set (field_set, field_id) && (attr_ptr0 [field_id]>>FS_ID_SHIFT) != 0)
			    nb++;
			}

			/* Ici is_root==SXTRUE */
			/* is_optional => nb == 0 || nb == 1 */
			/* !is_optional => nb == 1 */

			if (nb > 1 || nb == 0 && !is_optional) {
			  /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
			  is_locally_unconsistent = SXTRUE;
			  break;
			}
		      }
		      else {
			/* Ici is_root==SXTRUE */
			/* is_optional => nb == 0 || nb == 1 */
			/* !is_optional => nb == 1 */

			if (!is_optional) {
			  if (!fs_is_set (fs_id, field_id, &cur_val_ptr) /* non utilise' ... */
			      || ((*cur_val_ptr)>>FS_ID_SHIFT) == 0 /* ... ou vide */) {
			    /* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
			    is_locally_unconsistent = SXTRUE;
			    break;
			  }
			}
		      }
		    }
		  }
		}
	      }
	      else {
		/* Il doit y avoir un pred */
		/* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		is_locally_unconsistent = SXTRUE;
		break;
	      }
	    }
	    else {
	      if (val & CONSTRAINT_VAL) {
		/* Le 28/05/04 S'il y a incoherence sur la racine, on se contente de le signaler */
		is_locally_unconsistent = SXTRUE;
		break;
	      }
		  
	      /* On stocke aussi les elements des ensembles de f_structures (qui ne sont pas ds wfs) pour une verif
		 ulterieure */
	      static_field_kind = field_id2kind [field_id];

	      if (static_field_kind & STRUCT_KIND) {
		val >>= FS_ID_SHIFT;

		if (val > 0) {
		  if (static_field_kind == STRUCT_KIND) {
		    if (SXBA_bit_is_reset_set (fs_id_set, val))
		      PUSH (fs_id_stack, val);
		  }
		  else {
		    for (cur2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
			 cur2 < top2;
			 cur2++) {
		      val = XH_list_elem (XH_ufs_hd, cur2);
		
		      if (SXBA_bit_is_reset_set (fs_id_set, val))
			PUSH (fs_id_stack, val);
		    }
		  }
		}
	      }
	    }

	    cur++;
	  }
	}

	sxba_empty (fs_id_set);
      }
    }

    if (fs_id_stack [0]) {
      fs_id_stack [0] = 0;
      sxba_empty (fs_id_set);
    }
  }
  
#if EBUG
  if (!ret_val) {
    fputs ("Unconsistent\n", stdout);
  }
#endif /* EBUG */
    
  return ret_val;
}

/* On stocke ds les structures definitives tout ce qui se trouve ds wfs */
static SXINT
fill_fs ()
{
  SXINT             val, x, y, page_nb, delta_nb, field_id, fs_id, pair, bot, top, id, simulated_fs_id, son_nb;
  SXBA            field_set;
  SXBOOLEAN         done, is_shared;
  SXINT             *attr_ptr, *attr_ptr0;
  unsigned char   static_field_kind;

  shared_struct_nb = 0;

  /* Tri topologique, on commence par isoler celles qui ne dependent de rien */
  /* De l'id le +eleve vers le +petit, c,a ne marche pas car les structures nommees posent un pb */
  
  /* Ca doit marcher pour les structures cycliques !! */

  for (x = 0; x <= wfs_id; x++) {
    if (x == wfs_id2equiv [x]) {
      if (wfs_id2son_nb [x] == 0)
	PUSH (wfs_id_stack, x);
    }
  }

  done = SXFALSE;

  while (!done) {
    done = SXTRUE;

    while (!IS_EMPTY (wfs_id_stack) ) {
      x = POP (wfs_id_stack);

      /* entierement connu */
      /* Attention : il est possible que la structure "x" soit vide ds le cas de structures
	 lexicales nommees toutes vides */
      page_nb = AREA_PAGE (x);
      delta_nb = AREA_DELTA (x);

      field_set = wfs_field_set [page_nb] [delta_nb];
      attr_ptr0 = wfs2attr [page_nb] [delta_nb];

      field_id = 0;

      while ((field_id = sxba_scan_reset (field_set, field_id)) >= 0) {
	attr_ptr = attr_ptr0 + field_id;
	static_field_kind = field_id2kind [field_id];
	val = (*attr_ptr)>>FS_ID_SHIFT;

	if (val) {
	  /* Finalement, un champ cre'e' et non affecte' n'est pas conserve' */
	  if (static_field_kind == STRUCT_KIND) {
	    /* ... est positionne ds tous les cas, meme cyclique !! */
	    val = wfs2attr [AREA_PAGE (val)] [AREA_DELTA (val)] [0];
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      val = uwfs2ufs_id (val, SXFALSE);
	    }
#if ATOM_Aij || ATOM_Pij
	    else {
	      if (static_field_kind & Aij_KIND) {
		/* val reference une liste ds wfs_id_pred_list */
		/* On stocke les ensembles de Aij/Pij ds XH_ufs_hd */
		/* C'est une bonne ref a un ensemble non vide */
		val = uwfs2ufs_id (val, SXTRUE);
		/* val est l'identifiant d'une liste de ste ds XH_ufs_hd */
	      }
	    }
#endif /* ATOM_Aij || ATOM_Pij */
	  }

	  val = (val << FS_ID_SHIFT) + (*attr_ptr & (KIND_AND << MAX_FIELD_ID_SHIFT)) + field_id;
	  XH_push (XH_fs_hd, val);
	}
      }

      is_shared = x > 0 /* pas $$ ... */
	&& wfs_id2wfs_id_pred_list [x].first != wfs_id2wfs_id_pred_list [x].last /* ... et nb de predecesseurs > 1 */;

      if (is_shared) {
	/* Structure nommee (vide ou non) ... */
	/* ... on complete son code par son identifiant en negatif, c,a assure son unicite' */
	shared_struct_nb++;
	XH_push (XH_fs_hd, -shared_struct_nb); /* Unique ds $$ */
      }

      /* On fera une recherche dicho ds XH_fs_hd */
      if (!XH_set (&XH_fs_hd, &fs_id)) {
	/* nouveau */
	shared_fs_id2wfs_id [fs_id] = empty_shared_rhs; /* initialisation */
      }

      *attr_ptr0 = fs_id;

      if (x > 0) {
	/* x n'est pas $$ */
	/* "x" est calcule, toutes les structures qui en dependent sont maj */
	/* Non nommee ou nommee non vide => utilisee a un seul endroit */
	for (y = wfs_id2wfs_id_pred_list [x].first; y != 0; y = wfs_id_pred_list [y].next) {
	  if ((id = wfs_id_pred_list [y].wfs_id) >= 0) {
	    /* id peut etre negatif ds le cas cyclique */ /* A VOIR */
	    id = get_equiv (id);

	    if (--wfs_id2son_nb [id] == 0)
	      /* entierement calculee */
	      PUSH (wfs_id_stack, id);
	  }
	}
      }
    }

#if 0
    /* Ca pourra peut-etre etre utilise quand il y aura des structures cycliques ds les equations ... */
    if ((x = sxba_scan (cyclic_wfs_id_set, 0)) > 0) {
      /* Il y a des structures cycliques nommees */
      /* On simule les calculs pour donner les bons fs_id */
      done = SXFALSE;
      simulated_fs_id = XH_top (XH_fs_hd);

      for (y = 1; y <= wfs_id; y++) {
	if ((son_nb = wfs_id2son_nb [y]) != 0)
	  wfs_id2simulated_son_nb [y] = son_nb;
      }

      do {
	/* x est cyclique */
	/* On va simuler les calculs pour trouver les fs_id */
	/* en supposant que les occur vides sont calculables */
	for (y = wfs_id2wfs_id_pred_list [x].first; y != 0; y = wfs_id_pred_list [y].next) {
	  if ((id = wfs_id_pred_list [y].wfs_id) < 0) {
	    id = get_equiv (-id);
	    /* id est un predecesseur de x */
	    
	    if (--wfs_id2simulated_son_nb [id] == 0)
	      /* -id ne contenait plus qu'une occur vide d'une f_struct cyclique */
	      /* entierement calculee */
	      PUSH (simulated_wfs_id_stack, id);
	  }
	}
      } while ((x = sxba_scan (cyclic_wfs_id_set, 0)) > 0);

      while (!IS_EMPTY (simulated_wfs_id_stack)) {
	x = POP (simulated_wfs_id_stack);

	if (SXBA_bit_is_set (cyclic_wfs_id_set, x)) {
	  /* x est cyclique (et donc non vide), elle a l'id simulated_fs_id */
	  cyclic_wfs_id2simulated_fs_id [x] = simulated_fs_id;
	}

	simulated_fs_id++; /* Increment ds tous les cas */

	for (y = wfs_id2wfs_id_pred_list [x].first; y != 0; y = wfs_id_pred_list [y].next) {
	  if ((id = wfs_id_pred_list [y].wfs_id) >= 0) {
	    id = get_equiv (id);
	    /* id est un predecesseur de x */
	    
	    if (--wfs_id2simulated_son_nb [id] == 0) {
	      /* entierement calculee */
	      PUSH (simulated_wfs_id_stack, id);
	    }
	  }
	}
      }

      /* Ici toutes les f_structures, cycliques ou non ont rec,u leur identifiant */
      x = 0;

      while ((x = sxba_scan (cyclic_wfs_id_set, 0)) > 0) {
	fs_id = cyclic_wfs_id2simulated_fs_id [x];
	wfs2attr [AREA_PAGE (x)] [AREA_DELTA (x)] [0] = fs_id;

	for (y = wfs_id2wfs_id_pred_list [x].first; y != 0; y = wfs_id_pred_list [y].next) {
	  if ((id = wfs_id_pred_list [y].wfs_id) < 0) {
	    id = get_equiv (-id);
	    /* id est un predecesseur de x */
	
	    if (--wfs_id2son_nb [id] == 0)
	      /* entierement calculee */
	      PUSH (wfs_id_stack, id);
	  }
	}
      }
    }
#endif /* 0 */
  }

#if EBUG
  for (x = 0; x <= wfs_id; x++) {
    if (/* wfs_id2equiv [x] == x && */wfs_id2son_nb [x] != 0)
      sxtrap (ME, "fill_fs");
  }
#endif /* EBUG */

  return fs_id;
}

static SXINT
fill_heads (Aij, fs_id)
     SXINT Aij, fs_id;
{
  SXINT head, x;

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

    DPUSH (fs_id_dstack, head);
    fs_id_dstack [x]++; /* et un fs_id de plus ... */
  }

  return head;
}


/* Pour l'instant la priorite' d'une f_structure est le max des priorites de ses sous-structures */
/* On ne note pas les prio des fs_id deja vus, on les recalcule */
static SXINT
get_priority (fs_id)
     SXINT fs_id;
{
  SXINT x, max_priority, priority, prio, local_priority, bot, top, val, field_id, static_field_kind, bot2, cur2, top2;

  if (fs_id == 0)
    return 0;

  /* A FAIRE : Prudence sur les cycliques */
  max_priority = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;
    val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

    if (field_id == PRED_ID) {
      priority = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)+3);
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

    bot++;
  }

  return max_priority;
}


static void
print_f_structure (Xpq, fs_id, is_unconsistent)
     SXINT     Xpq, fs_id;
     SXBOOLEAN is_unconsistent;
{
  SXINT             bot, top, bot2, cur2, top2, bot3, cur3, top3, max_priority, priority;
  SXINT             val, field_id, field_kind, sous_cat_id, atom_id, local_atom_id, x, Tpq, T, p, q, id;
  SXINT             *local2atom_id;
  SXBOOLEAN         is_optional, is_first, is_first2, is_first3, is_main_struct;
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
	print_symb (sxstdout, Tij2T (Xpq), Tij2i (Xpq), Tij2j (Xpq));
      }
      else {
	print_symb (sxstdout, Aij2A (Xpq), Aij2i (Xpq), Aij2j (Xpq));
      }

      printf ("= %i/ = [...]\n", Xpq);
      return;
    }

    /* Pour l'instant la priorite' d'une f_structure est le max des priorites de ses sous-structures */
    max_priority = 0;
    is_main_struct = SXTRUE;
    x = 0; /* indice ds fs_id_stack */

    do {
      bot = XH_X (XH_fs_hd, fs_id);
      top = XH_X (XH_fs_hd, fs_id+1);

      if (is_main_struct) {
	is_main_struct = SXFALSE;
	printf ("F%i/", fs_id);

	if (Xpq > spf.outputG.maxxnt) {
	  /* Tpq */
	  Xpq -= spf.outputG.maxxnt;
	  print_symb (sxstdout, Tij2T (Xpq), Tij2i (Xpq), Tij2j (Xpq));
	}
	else {
	  print_symb (sxstdout, Aij2A (Xpq), Aij2i (Xpq), Aij2j (Xpq));
	}

	printf ("= %i/ = [", Xpq);
      }
      else {
	if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0) {
	  top--;

	  printf ("F%i(shared) = [", fs_id);
	}
	else
	  printf ("F%i = [", fs_id);
      }

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

	if (field_id == PRED_ID) {
	  SXINT *dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));
      
	  if ((priority = dpv_ptr [3]) > max_priority)
	    max_priority = priority;

	  fputs ("pred =", stdout);

	  if (field_kind & ASSIGN_CONSTRAINT_KIND)
	    fputs ("c", stdout);

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
	      p = mlstn2dag_state [p];
	      q = mlstn2dag_state [q];
	      */

	      printf (", %s[%i..%i]", spf.inputG.tstring [T], p, q);
	    }
	  } 
	  
	  fputs ("\"", stdout);
	}
	else {
	    printf ("%s ", field_id2string [field_id]);

	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND)
	    fputs ("= {", stdout);
	  else {
	    if (field_kind & ASSIGN_CONSTRAINT_KIND)
	      fputs ("=c ", stdout);
	    else
	      fputs ("= ", stdout);
	  }

	  if (static_field_kind & ATOM_KIND) {
	    if (static_field_kind & UNBOUNDED_KIND) {
#if ATOM_Aij || ATOM_Pij
	      if (static_field_kind & Aij_KIND) {
		fputs ("{", stdout);

		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  if (cur2 > bot2)
		    fputs (", ", stdout);

		  id = XH_list_elem (XH_ufs_hd, cur2);

		  if (id > spf.outputG.maxxnt) { 
		    id -= spf.outputG.maxxnt; /* Pij */
		    printf ("R_%i_%i", id, spf.outputG.lhs [id].init_prod);
		  }
		  else
		    printf ("N%i", id /* Aij */);
		}
		
		fputs ("}", stdout);
	      }
	      else
#endif /* ATOM_Aij || ATOM_Pij */
		printf ("%s", atom_id2string [val]);
	    }
	    else {
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

		  printf ("%s", atom_id2string [atom_id]);
		}
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
		if (val == 0)
		  fputs ("[]}", stdout);
		else {
		  for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		       cur2 < top2;
		       cur2++) {
		    if (cur2 > bot2)
		      fputs (", ", stdout);
		
		    val = XH_list_elem (XH_ufs_hd, cur2);
		
		    if (SXBA_bit_is_reset_set (fs_id_set, val))
		      PUSH (fs_id_stack, val);

		    printf ("[]F%i", val);
		  }
		
		  fputs ("}", stdout);
		}
	      }
	    }
	  }
	}

	bot++;
      }
 
      fputs ("]", stdout);

      if (++x > fs_id_stack [0])
	/* Pas de nl pour Priority ... */
	fs_id = 0;
      else {
	fputs ("\n", stdout);
	fs_id = fs_id_stack [x];
      }

    } while (fs_id);

    if (max_priority) {
      if (max_priority == MAX_PRIORITY)
	fputs (", Priority = maximum\n", stdout);
      else
	printf (", Priority = %i\n", max_priority);
    }
    else
      fputs ("\n", stdout);

    sxba_empty (fs_id_set);
    fs_id_stack [0] = 0;
  }
  else {
    fputs ("F0/", stdout);

    if (Xpq > spf.outputG.maxxnt) {
      /* Tpq */
      Xpq -= spf.outputG.maxxnt;
      print_symb (sxstdout, Tij2T (Xpq), Tij2i (Xpq), Tij2j (Xpq));
    }
    else {
      print_symb (sxstdout, Aij2A (Xpq), Aij2i (Xpq), Aij2j (Xpq));
    }

    printf ("= %i/ = []\n", Xpq);
  }
}


/* on travaille ds wfs */
/* positionne le champ field_id au niveau cur_wfs_id de wfs */
/* retourne SXTRUE ssi field_id existe deja */
static SXBOOLEAN
wfs_set (cur_wfs_id, field_id, ret_val)
     SXINT cur_wfs_id, field_id, **ret_val;
{
  SXINT page_nb, delta_nb, val;

  cur_wfs_id = get_equiv (cur_wfs_id); /* En fait on est la` !! */

  page_nb = AREA_PAGE (cur_wfs_id);
  delta_nb = AREA_DELTA (cur_wfs_id);
  *ret_val = wfs2attr [page_nb] [delta_nb] + field_id;

  if (SXBA_bit_is_reset_set (wfs_field_set [page_nb] [delta_nb], field_id)) {
    /* Nouveau champ */
    if (field_id2kind [field_id] == STRUCT_KIND) {
      val = get_a_new_wfs_id (); /* wfs_id == val */
      put_new_pred_wfs_id (wfs_id, cur_wfs_id, *ret_val);
    }
    else
      val = 0;

    **ret_val = (val<<FS_ID_SHIFT)+field_id;

    return SXFALSE;
  }

  return SXTRUE;
}

/* Comme wfs_set, excepte' qu'on ne cree pas de nouveau champ */
static SXBOOLEAN
re_wfs_set (cur_wfs_id, field_id, ret_val)
     SXINT             cur_wfs_id, field_id, **ret_val;
{
  SXINT             page_nb, delta_nb;
      
  cur_wfs_id = get_equiv (cur_wfs_id); /* En fait on est la` !! */

  page_nb = AREA_PAGE (cur_wfs_id);
  delta_nb = AREA_DELTA (cur_wfs_id);

  if (!SXBA_bit_is_set (wfs_field_set [page_nb] [delta_nb], field_id))
    return SXFALSE;

  *ret_val = wfs2attr [page_nb] [delta_nb] + field_id;

  return SXTRUE;
}

#if EBUG
static SXINT *ppcs;

static VARSTR
code2vstr (vstr)
     VARSTR vstr;
{
  SXINT     oper, suffix_id, field_id, val, local_atom_id, atom_id;
  SXINT     *local2atom_id;
  char    string [12];
  SXBOOLEAN is_first;

  oper = *(--ppcs);

  if (oper >= 0)
    sxtrap (ME, "code2vstr");

  switch (oper) {
  case OPERATOR_SPACE:
  case OPERATOR_DOT:
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, " ");
    vstr = code2vstr (vstr);
    break;
    
  case OPERATOR_MINUS:
    suffix_id = *(--ppcs);
    vstr = varstr_catenate (vstr, "(");
    vstr = code2vstr (vstr);
    vstr = varstr_catenate (vstr, ")-");
    vstr = varstr_catenate (vstr, suffix_id2string [suffix_id]);
    break;

  case OPERATOR_LEX_REF:
    vstr = varstr_catenate (vstr, "lex ");

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

    if (field_id == 0) {
      /* UNBOUNDED_KIND */
      vstr = varstr_catenate (vstr, atom_id2string [val]);
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

	  vstr = varstr_catenate (vstr, atom_id2string [atom_id]);
	}
      }
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
    vstr = varstr_catenate (vstr, "[]");

    break;

  default:
    sxtrap (ME, "code2vstr");
  }

  return vstr;
}

static void
print_equation_code (xcode)
     SXINT xcode;
{
  SXINT  oper;
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
#endif /* EBUG */



/*
  -1 : skip the current equation
   0 : SXFALSE
   1 : SXTRUE
 */
static SXINT
eval (local_ptr, local_kind)
     SXINT **local_ptr, *local_kind;
{
  SXINT    field_id, kind, i, ref, Xpq, left_kind, val, head, oper, string_id, local_atom_id, atom_id, static_field_kind;
  SXINT    *val_ptr, *head_ptr, *field_id_ptr, *ref_ptr;
  SXINT    ret_val, p, q;

  oper = *(--pcs);

  if (oper < 0) {
    /* C'est un operateur */ 
    switch (oper) {
    case OPERATOR_SPACE:
    case OPERATOR_DOT:
      head_ptr = &head; /* finesse */

      if ((ret_val = eval (&head_ptr, &left_kind)) <= 0)
	return ret_val;

      /* Attention, au retour, head_ptr peut ne plus pointer vers head!! */
      head = *head_ptr;

      field_id_ptr = &field_id;

      if ((ret_val = eval (&field_id_ptr, &kind)) <= 0)
	return ret_val;

      /* head est une val typee */
      head >>= FS_ID_SHIFT;
      /* head est un [w]fs_id */

#if EBUG
      if ((left_kind & (ATOM_VAL+ATOM_FIELD)) || (kind & ~(ATOM_FIELD+STRUCT_FIELD+UNBOUNDED_STRUCT_FIELD)))
	sxtrap (ME, "eval");
#endif /* EBUG */

      i = left_kind>>REF_SHIFT;

      if (i == 0) {
	/* On est ds $$ */
	/* head est un wfs_id */
	*local_kind = kind + (wfs_set (head, field_id, local_ptr) ? OLD_FIELD : NEW_FIELD);

	return 1;
      }
	
      /* On est ds un $i */
      /* head est un fs_id */
      if (!fs_is_set (head, field_id, local_ptr)) {
	/* Champ inconnu ds un $i ... */
	/* On suppose qu'on ne cree jamais de champ sur les fils en $i!! */
#if EBUG
	printf (" <The field $%i ... %s is unknown> ", i, field_id2string [field_id]);
#endif /* EBUG */
	return -1; /* L'equation est seulement abandonnee ... */
	/* ... sauf si la valeur de ce champ sert a construire un nouveau champ auquel cas
	 l'unification doit echouer */
      }

      *local_kind = (i<<REF_SHIFT)+OLD_FIELD+kind;

      return 1;

    case OPERATOR_MINUS:
      /* ($i X* f)-string */
      string_id = *(--pcs); /* C'est le ste de string ds le constructeur !! */ 

      ref_ptr = &ref;

      if ((ret_val = eval (&ref_ptr, &kind)) <= 0) {
	return 0; /* echec le 04/08/04 */
#if 0
	return ret_val;
#endif /* 0 */
      }

      ref = *ref_ptr;
      field_id = ref & FIELD_AND;

      if ((kind & OLD_FIELD) == 0) {
	/* Le champ doit avoir une valeur !! */
	fprintf (sxstderr, "The field $%i ... %s must be set.\n", kind>>REF_SHIFT, field_id2string [field_id]);
	return 0;
      }

      /* kind == (i<<REF_SHIFT)+ATOM_FIELD */
      ref >>= FS_ID_SHIFT;

#if EBUG
      if ((kind & ATOM_FIELD) == 0 || ref == 0)
	sxtrap (ME, "eval");
#endif /* EBUG */

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
	  fprintf (sxstderr, "The field $%i ... %s has a illegal conjonctive value.\n", kind>>REF_SHIFT, field_id2string [field_id]);
	  return 0;
	}

	atom_id = field_id2atom_field_id [field_id] [local_atom_id];
      }

      val = XxY_is_set (&dfield_pairs, atom_id, string_id);

#if EBUG
      if (val == 0)
	sxtrap (ME, "eval");
#endif /* EBUG */

      **local_ptr = field_id = dfield_pairs2field_id [val];

      *local_kind = (static_field_kind & STRUCT_KIND)
	? (static_field_kind & UNBOUNDED_KIND
	   ? UNBOUNDED_STRUCT_FIELD
	   : STRUCT_FIELD)
	: ATOM_FIELD;

      return 1;
      
    case OPERATOR_LEX_REF: 
      /* Ici, meme code que OPERATOR_REF */
    case OPERATOR_REF: 
      ref_ptr = &ref;

      if ((ret_val = eval (&ref_ptr, &kind)) <= 0)
	return ret_val;

      *local_kind = (ref << REF_SHIFT)+DOL_REF;

      if (ref > 0) {
	/* Si un fils est vide sa semantique est vide... */
	head = rhs_pos2fs [ref]; /* Le bon (Xpq, p) */
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

	if (p < 0 || q < 0 || p == q) {
	  /*... erreur ou vide */
	  return -1;
	}

	SXBA_1_bit (doli_used_set, ref); /* Pour connaitre les doli utilises */

	head = XxY_Y (heads, head); /* fs_id */
      }
      else
	/* $$ */
	head = 0; /* head == 0 */

      head <<= FS_ID_SHIFT;

      **local_ptr = head;

      return 1;

    case OPERATOR_EMPTY_STRUCT:
      /* On est ds le cas "$$ ... = []" */
      *local_kind = EMPTY_STRUCT;
      **local_ptr = 0;
      return 1;

    case OPERATOR_FIELD:
      field_id_ptr = &field_id;

      if ((ret_val = eval (&field_id_ptr, &kind)) <= 0)
	return ret_val;

      static_field_kind = field_id2kind [field_id];

      *local_kind = (static_field_kind & STRUCT_KIND)
	? (static_field_kind & UNBOUNDED_KIND
	   ? UNBOUNDED_STRUCT_FIELD
	   : STRUCT_FIELD)
	: ATOM_FIELD;

      **local_ptr = field_id;
      
      /* ATOM_KIND */

      return 1;

    case OPERATOR_ATOM:
      field_id = *(--pcs); /* inutilise' ici */
      val_ptr = &val;

      if ((ret_val = eval (&val_ptr, &kind)) <=0)
	return ret_val;

      *local_kind = ATOM_VAL;
      **local_ptr = val;

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

static SXINT
re_concat (left_top, right_top)
     SXINT left_top, right_top;
{
  SXINT prev_top, x, y, head, left_kind, i, field_id, kind, new_top;
  SXINT *ptr;

  if (left_top == 0)
    return right_top;

  prev_top = 0;

  for (x = left_top; x != 0; x = re_results [x].next) {
    head = *(re_results [x].ptr) >> FS_ID_SHIFT;
    left_kind = re_results [x].kind;
    i = left_kind>>REF_SHIFT;

    for (y = right_top; y != 0; y = re_results [y].next) {
      field_id = *(re_results [y].ptr);
      kind = re_results [y].kind;

      if (i == 0) {
	/* On est ds $$ */
	/* head est un wfs_id */
	if (re_wfs_set (head, field_id, &ptr)) {
	  /* field_id existe ds $$ */
	  new_top = re_results_get_new_top ();
	  re_results [new_top].next = prev_top;
	  re_results [new_top].kind = kind + OLD_FIELD;
	  re_results [new_top].ptr = ptr;
	  prev_top = new_top;
	}
      }
      else {
	/* On est ds un $i */
	/* head est un fs_id */
	if (fs_is_set (head, field_id, &ptr)) {
	  /* field_id existe ds $i */
	  new_top = re_results_get_new_top ();
	  re_results [new_top].next = prev_top;
	  re_results [new_top].kind = (i<<REF_SHIFT) + kind + OLD_FIELD;
	  re_results [new_top].ptr = ptr;
	  prev_top = new_top;
	}
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
static SXINT
re_eval (input)
     SXINT input;
{
  SXINT     oper, left_ret_val, right_ret_val, string_id, prev_top, x, ref, kind, field_id, atom_id, local_atom_id, val, new_top;
  SXINT     *store_pcs, next, last_ret_index, ret_val, prev_ret_val, static_field_kind;

  oper = *(--pcs);

  if (oper < 0) {
    /* C'est un operateur */ 
    switch (oper) {
    case OPERATOR_SPACE:
    case OPERATOR_DOT:
      if ((left_ret_val = re_eval (input)) <= 0)
	return left_ret_val;

      return re_eval (left_ret_val);

    case OPERATOR_MINUS:
      /* ($i X* f)-string */
      string_id = *(--pcs); /* C'est le ste de string ds le constructeur !! */ 

      if ((left_ret_val = re_eval ()) <= 0)
	return left_ret_val;

      prev_top = 0;

      for (x = left_ret_val; x != 0; x = re_results [x].next) {
	ref = *(re_results [x].ptr);
	kind = re_results [x].kind;

	if (kind & NEW_FIELD) {
	  /* Le champ doit avoir une valeur !! */
	  field_id = ref & FIELD_AND;
	  ref >>= FS_ID_SHIFT;

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
	      fprintf (sxstderr, "The field $%i ... %s has a illegal conjonctive value.\n", kind>>REF_SHIFT, field_id2string [field_id]);
	      return 0;
	    }

	    atom_id = field_id2atom_field_id [field_id] [local_atom_id];
	  }

	  val = XxY_is_set (&dfield_pairs, atom_id, string_id);

#if EBUG
	  if (val == 0)
	    sxtrap (ME, "re_eval");
#endif /* EBUG */

	  new_top = re_results_get_new_top ();
	  re_results [new_top].next = prev_top;
	  *(re_results [new_top].ptr) = field_id = dfield_pairs2field_id [val];

	  static_field_kind = field_id2kind [field_id];

	  re_results [new_top].kind = (static_field_kind & STRUCT_KIND)
	    ? (static_field_kind & UNBOUNDED_KIND
	       ? UNBOUNDED_STRUCT_FIELD
	       : STRUCT_FIELD)
	    : ATOM_FIELD;

	  prev_top = new_top;
	}
      }

      return re_concat (input, prev_top);    

    case OPERATOR_KLEENE:
      /* $i P (...)* */
      store_pcs = pcs;
      prev_ret_val = input;

      /* On evalue le bloc */
      for (;;) {
	pcs = store_pcs;

	if ((left_ret_val = re_eval (prev_ret_val)) == -1)
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

      return input;

    case OPERATOR_OR:
      if ((left_ret_val = re_eval (input)) == -1)
	return -1;

      if ((right_ret_val = re_eval (input)) == -1)
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

    case OPERATOR_LEX_REF: 
    case OPERATOR_REF: 
    case OPERATOR_FIELD:
    case OPERATOR_ATOM:
    case OPERATOR_EMPTY_STRUCT:
      /* On peut utiliser eval () !! */
      /* input ne sert a rien */
      pcs++;
      DPUSH (re_tank, 0); /* On reserve une place en DTOP (re_tank) ... */
      new_top = re_results_get_new_top ();
      re_results [new_top].next = 0;
      re_results [new_top].ptr = re_tank+DTOP (re_tank); /* ... et on la reference ici */

      if ((ret_val = eval (&(re_results [new_top].ptr), &(re_results [new_top].kind))) <= 0)
	return ret_val;

      if (oper == OPERATOR_FIELD && input != 0)
	new_top = re_concat (input, new_top);

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



static SXBOOLEAN is_constrained_unification;
static SXBOOLEAN unify ();

/* On unifie les ensembles de f_structures reperes par left_val_ptr et right_val_ptr */
static SXBOOLEAN
unbounded_structure_unification (left_val_ptr, right_val_ptr, right_dol)
     SXINT *left_val_ptr, *right_val_ptr, right_dol;
{
  SXINT left_id, right_id, bot, top, cur, prev_top;

  /* Un ensemble de f_structures qui ne contient qu'une f_structure vide (-1 !!) est vide */

  left_id = *left_val_ptr >> FS_ID_SHIFT;
  right_id = *right_val_ptr >> FS_ID_SHIFT;

  if (left_id == 0) {
    if (right_id == 0) {
#if EBUG
      if (right_dol != 0)
	/* Ensemble vide en fs n'est pas 0 !! */
	sxtrap (ME, "unbounded_structure_unification");
#endif /* EBUG */
      
      /* On cree un ensemble vide */
      top = get_new_top_pred_wfs_id ();
      wfs_id_pred_list [top].next = 0;
      wfs_id_pred_list [top].wfs_id = -1;
      *right_val_ptr = (top << FS_ID_SHIFT) + (*right_val_ptr & FS_ID_AND);
    }
    else {
      if (right_dol == 0) {
	top = (*right_val_ptr >> FS_ID_SHIFT);
      }
      else {
	/* On cree un ensemble et on recopie */
	bot = XH_X (XH_ufs_hd, right_id);
	top = XH_X (XH_ufs_hd, right_id+1);

	if (bot == top) {
	  /* vide */
	  /* On cree un ensemble vide */
	  top = get_new_top_pred_wfs_id ();
	  wfs_id_pred_list [top].next = 0;
	  wfs_id_pred_list [top].wfs_id = -1;
	}
	else {
	  prev_top = 0;

	  for (cur = top-1; cur >= bot; cur--) {
	    top = get_new_top_pred_wfs_id ();
	    wfs_id_pred_list [top].next = prev_top;
	    prev_top = top;
	    wfs_id_pred_list [top].wfs_id = XH_list_elem (XH_ufs_hd, cur);
	  }
	}
      }
    } 
	  
    *left_val_ptr = (top << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);
  }
  else {
    if (right_id == 0) {
      if (right_dol == 0) {
	top = (*left_val_ptr >> FS_ID_SHIFT);
	*right_val_ptr = (top << FS_ID_SHIFT) + (*right_val_ptr & FS_ID_AND);
      }
      /* else rien */
    }
    else {
      if (right_dol == 0) {
	/* On est ds $$ a gauche et a droite */
	prev_top = (*left_val_ptr >> FS_ID_SHIFT);

	while (prev_top = wfs_id_pred_list [top = prev_top].next != 0);
	 
	wfs_id_pred_list [top].next = prev_top = (*right_val_ptr >> FS_ID_SHIFT);
	*right_val_ptr = (prev_top << FS_ID_SHIFT) + (*right_val_ptr & FS_ID_AND);
      }
      else {
	/* Recopie de $i vers $$ */
	bot = XH_X (XH_ufs_hd, right_id);
	top = XH_X (XH_ufs_hd, right_id+1);
	prev_top = (*left_val_ptr >> FS_ID_SHIFT);

	for (cur = top-1; cur >= bot; cur--) {
	  top = get_new_top_pred_wfs_id ();
	  wfs_id_pred_list [top].next = prev_top;
	  prev_top = top;
	  wfs_id_pred_list [top].wfs_id = XH_list_elem (XH_ufs_hd, cur);
	}	  

	*left_val_ptr = (top << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);
      }
    }
  }

  return SXTRUE;
}

/* On unifie les structures left_wfs_id et right_wfs_id dans left_wfs_id */
/* left_wfs_id est le representant de la classe d'equivalence.
   Il en est de meme de right_wfs_id si right_dol==0 */
static SXBOOLEAN
structure_unification (left_wfs_id, right_wfs_id, right_dol, checking)
     SXINT     left_wfs_id, right_wfs_id, right_dol;
     SXBOOLEAN checking;
{
  SXINT     field_id, field_kind, bot, cur, top, static_field_kind;
  SXINT     left_page_nb, left_delta_nb, *left_attr_ptr0, *left_attr_ptr;
  SXINT     right_page_nb, right_delta_nb, *right_attr_ptr0, *right_attr_ptr;
  SXINT     right_id, new_wfs_id, bot2, top2, page_nb;
  SXINT     *ptr;
  SXBA    left_field_set, right_field_set;
  SXBOOLEAN ret_val, has_right_field, is_shared, is_new_wfs_id;

#if EBUG
  if (left_wfs_id != wfs_id2equiv [left_wfs_id])
    sxtrap (ME, "structure_unification");
#endif /* EBUG */

#if EBUG
  if (right_dol == 0) {
    if (right_wfs_id != wfs_id2equiv [right_wfs_id])
      sxtrap (ME, "structure_unification");
  }
#endif /* EBUG */

  if (right_dol == 0 && left_wfs_id == right_wfs_id)
    return SXTRUE;

  /* Attention : je suppose (ds un premier temps !!) que l'unification ds left_wfs_id n'est pas cyclique */
  if (!SXBA_bit_is_reset_set (wfs_id_set, left_wfs_id))
    sxtrap (ME, "cyclic structure unification is not (yet) implemented");

  left_page_nb = AREA_PAGE (left_wfs_id);
  left_delta_nb = AREA_DELTA (left_wfs_id);
  left_field_set = wfs_field_set [left_page_nb] [left_delta_nb];
  left_attr_ptr0 = wfs2attr [left_page_nb] [left_delta_nb];
  ret_val = SXTRUE;

  if (right_dol == 0) {
    /* right_wfs_id est ds wfs */
    /* On unifie donc right_wfs_id ds left_wfs_id */
    right_page_nb = AREA_PAGE (right_wfs_id);
    right_delta_nb = AREA_DELTA (right_wfs_id);
    right_field_set = wfs_field_set [right_page_nb] [right_delta_nb];
    right_attr_ptr0 = wfs2attr [right_page_nb] [right_delta_nb];
    field_id = 0;

    while ((field_id = sxba_scan (right_field_set, field_id)) > 0) {
      static_field_kind = field_id2kind [field_id];
      left_attr_ptr = left_attr_ptr0+field_id;
      right_attr_ptr = right_attr_ptr0+field_id;

      if (!SXBA_bit_is_set (left_field_set, field_id)) {
	/* field_id est a droite et pas a gauche */
	if (!checking) {
	  SXBA_1_bit (left_field_set, field_id);
	  *left_attr_ptr = *right_attr_ptr;

	  if (static_field_kind == STRUCT_KIND) {
	    if ((new_wfs_id = (*right_attr_ptr >> FS_ID_SHIFT)) == 0) {
	      /* On suppose que ca veut dire que cette structure est vide (et non pas qu'il s'agit de $$ !!) */
	      /* Normalement ds wfs une structure vide ne se represente pas par 0 mais par un id>0 qui
		 repere un field_set vide */
	      printf ("System error : $$ ... %s is null!!\n", field_id2string [field_id]);
	      sxtrap (ME, "structure_unification");
	    }

	    put_new_pred_wfs_id (new_wfs_id, left_wfs_id, left_attr_ptr);
	  }
	}
	/* Ds le cas checking est_ce ose' de ne pas positionner left_field_set ? */
      }
      else {
	/* field_id est a gauche et a droite */
	if (static_field_kind == STRUCT_KIND) {
	  if ((*left_attr_ptr >> FS_ID_SHIFT) == 0 || (*right_attr_ptr >> FS_ID_SHIFT) == 0) {
	    /* On suppose que ca veut dire que cette structure est vide (et non pas qu'il s'agit de $$ !!) */
	    printf ("System error : $$ ... %s is null!!\n", field_id2string [field_id]);
	    sxtrap (ME, "structure_unification");
	  }
	}

	field_kind = ((static_field_kind & STRUCT_KIND)
		      ? (static_field_kind & UNBOUNDED_KIND
			 ? UNBOUNDED_STRUCT_FIELD
			 : STRUCT_FIELD)
		      : ATOM_FIELD) + (right_dol << REF_SHIFT) + OLD_FIELD;

	if (!unify (left_attr_ptr, field_kind, right_attr_ptr, field_kind, checking)) {
	  ret_val = SXFALSE;
	  break;
	}
      }
    }

    /* Normalement c'est fait ds l'appelant (unify ()) de`s le retour */
    if (ret_val && !checking) {
      /* On vient d'unifier 2 structures ds wfs, elles sont donc equivalentes */
      merge (left_wfs_id, right_wfs_id);
    }
  }
  else {
    /* right_dol > 0 => right_wfs_id est ds fs_id */
    /* On unifie donc right_wfs_id ds left_wfs_id */
    bot = XH_X (XH_fs_hd, right_wfs_id);
    top = XH_X (XH_fs_hd, right_wfs_id+1);

    if (is_shared = (XH_list_elem (XH_fs_hd, top-1) < 0 /* shared */))
      top--;
  
    for (cur = bot; cur < top; cur++) {
      right_attr_ptr = &(XH_list_elem (XH_fs_hd, cur));
      field_id =  *right_attr_ptr & FIELD_AND;
      static_field_kind = field_id2kind [field_id];
      left_attr_ptr = left_attr_ptr0+field_id;

      if (!SXBA_bit_is_set (left_field_set, field_id)) {
	/* field_id n'est pas a gauche */
	if (!checking) {
	  SXBA_1_bit (left_field_set, field_id);

	  if (static_field_kind == STRUCT_KIND) {
	    /* C'est une structure qu'il faut recopier de fs ds wfs */
	    right_id = *right_attr_ptr >> FS_ID_SHIFT;
	    /* right_id est ds fs */ 
	    /* Si cette structure est partagee et a deja ete recopiee/unifiee avec un wfs_id, on peut
	       utiliser directement ce wfs_id */
	    if (
#if 1
		/* Le 06/05/04 ESSAI : Les [sous-]structures des $i ne sont instanciees qu'une seule fois ds un bloc d'equations */
		/* Echec : Voir commentaire ci_dessous */
		XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, right_id+1)-1) < 0 /* shared */ && 
#endif /* 1 */
		(new_wfs_id = shared_fs_id2wfs_id [right_id].rhs [right_dol]) >= 0 /* Pas le 1er coup */) {
	      new_wfs_id = get_equiv (new_wfs_id);
	    }
	    else {
	      new_wfs_id = get_a_new_wfs_id ();

	      if (!structure_unification (new_wfs_id, right_id, right_dol, checking)) {
		ret_val = SXFALSE;
		break;
	      }
	    }

	    *left_attr_ptr = (new_wfs_id<<FS_ID_SHIFT) + (*right_attr_ptr&FS_ID_AND);
	    put_new_pred_wfs_id (new_wfs_id, left_wfs_id, left_attr_ptr);
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND || (static_field_kind & Aij_KIND)) {
	      *left_attr_ptr = (*right_attr_ptr & FS_ID_AND);
	
#if ATOM_Aij || ATOM_Pij
	      if (static_field_kind & Aij_KIND) {
		if (!unify (left_attr_ptr,
			    ATOM_FIELD+NEW_FIELD,
			    right_attr_ptr,
			    ATOM_FIELD+OLD_FIELD+(right_dol<<REF_SHIFT),
			    checking)) {
		  /* Ca doit toujours marcher ... */
		  sxtrap (ME, "structure_unification");
		}
	      }
	      else
#endif /* ATOM_Aij || ATOM_Pij */
		{
		  if (!unify (left_attr_ptr,
			      UNBOUNDED_STRUCT_FIELD+NEW_FIELD,
			      right_attr_ptr,
			      UNBOUNDED_STRUCT_FIELD+OLD_FIELD+(right_dol<<REF_SHIFT),
			      checking)) {
		    ret_val = SXFALSE;
		    break;
		  }
		}
	    }
	    else {
	      *left_attr_ptr = *right_attr_ptr;
	    }
	  }
	}
      }
      else {
	/* field_id est a gauche et a droite */
	if (static_field_kind == STRUCT_KIND) {
	  if ((*left_attr_ptr >> FS_ID_SHIFT) == 0) {
	    /* On suppose que ca veut dire que cette structure est vide (et non pas qu'il s'agit de $$ !!) */
	    /* Normalement ds wfs une structure vide ne se represente pas par 0 mais par un id>0 qui
	       repere un field_set vide */
	    printf ("System error : $$ ... %s is null!!\n", field_id2string [field_id]);
	    sxtrap (ME, "structure_unification");
	  }
	}

	field_kind = ((static_field_kind & STRUCT_KIND)
		      ? (static_field_kind & UNBOUNDED_KIND
			 ? UNBOUNDED_STRUCT_FIELD
			 : STRUCT_FIELD)
		      : ATOM_FIELD) + OLD_FIELD;

	if (!unify (left_attr_ptr, field_kind, right_attr_ptr, field_kind + (right_dol << REF_SHIFT), checking)) {
	  ret_val = SXFALSE;
	  break;
	}
      }
    }

    if (ret_val && !checking
#if 1
	/* Le 06/05/04 ESSAI : Les [sous-]structures des $i ne sont instanciees qu'une seule fois ds un bloc d'equations */
	/* Echec : car les [sous-]structures egales et non partagees (memes vides) d'un $i se partagent, ce qui est faux */
	/* Idee : calculer en // avec la recherche des champs (ds fs_is_set) des chemins de la forme c = (...(($i, X1), X2), ..., Xp)
	   ds un XxY ou les Xi sont des field_id et associer en T[c] le wfs_id ou` a ete instancie' le fs_id de la
	   structure reperee par $i X1 X2 ... Xp */
	&& is_shared
#endif /* 1 */
	) {
      /* unification OK et ...
      /* ... right_wfs_id est un id ds fs qui repere une structure partagee */
      /* On regarde si c'est la 1ere instanciation */
      if ((new_wfs_id = *(ptr = &(shared_fs_id2wfs_id [right_wfs_id].rhs [right_dol]))) == -1) {
	*ptr = new_wfs_id = left_wfs_id;
	DPUSH (shared_fs_id_stack, right_wfs_id);
	DPUSH (shared_fs_id_stack, right_dol);
      }
#if EBUG
      else {
	/* Les instanciations autres que la 1ere doivent etre capturees au debut de structure_unification () */
	/* En fait left_wfs_id et new_wfs_id doivent etre equivalents */
	sxtrap (ME, "structure_unification");
      }
#endif /* EBUG */
    }
  }

  SXBA_0_bit (wfs_id_set, left_wfs_id);

  return ret_val;
}



/* Cas $i X* < $$ Y+ */
static SXBOOLEAN
unify_in (left_val_ptr, left_kind, right_val_ptr, right_kind)
     SXINT     *left_val_ptr, left_kind, *right_val_ptr, right_kind;
{
  SXINT top, left_id, right_id;

#if EBUG
  /* Pas de structure vide provenant d'un fils */
  if ((left_kind & NEW_FIELD) && (left_kind>>REF_SHIFT))
     sxtrap (ME, "unify_in");
#endif /* EBUG */

  /* J'utilise "pred_list" !! */
  top = get_new_top_pred_wfs_id ();

  if (right_kind & NEW_FIELD) {
    /* Y est nouveau, on lui affecte left_val */
    wfs_id_pred_list [top].next = 0;
  }
  else {
    /* On verifiera les duplicats au passage ds fs */
    wfs_id_pred_list [top].next =  (*right_val_ptr >> FS_ID_SHIFT);
  }

  if ((left_kind>>REF_SHIFT) == 0) {
    /* $$ X* < $$ Y+ */
    left_id = get_equiv (*left_val_ptr >> FS_ID_SHIFT);
    right_id = get_equiv (*right_val_ptr >> FS_ID_SHIFT);
    /* right_id ne pourra s'evaluer que si la lhs l'est */
    put_new_pred_wfs_id (left_id, right_id, NULL);
  }

  /* On "marque" les ref aux wfs_id */
  wfs_id_pred_list [top].wfs_id = (*left_val_ptr >> FS_ID_SHIFT) + ((left_kind>>REF_SHIFT) == 0 ? X80 : 0);
  *right_val_ptr = (top << FS_ID_SHIFT) + (*right_val_ptr & FS_ID_AND);
  
  return SXTRUE;
}



static SXBOOLEAN
unify (left_val_ptr, left_kind, right_val_ptr, right_kind, checking)
     SXINT *left_val_ptr, left_kind, *right_val_ptr, right_kind;
     SXBOOLEAN checking;
{
  SXINT             right_dol, main_head, val, left_id, right_id, new_wfs_id, top;
  SXINT             *dpv_lptr, *dpv_rptr;
  unsigned char   left_static_field_kind;
    
  if (right_kind == ATOM_VAL) {
    /* $$ ... X = "-" */
#if EBUG
    if ((left_kind & (ATOM_FIELD)) == 0)
      sxtrap (ME, "unify");
#endif /* EBUG */

#if ATOM_Aij || ATOM_Pij
    if (*right_val_ptr == ATOM_Aij || *right_val_ptr == ATOM_Pij) {
    /* $$ ... X = Aij | Pij et X est bien un champ atomique unbounded */
      if (!checking) {
#if 0
	SXINT ste;
	char str [32];

	if (*right_val_ptr == ATOM_Aij)
	  sprintf (str, "N%i", AIJ);
	else
	  sprintf (str, "R_%i_%i", PIJ, spf.outputG.lhs [PIJ].init_prod);

	ste = sxstrsave (str);

	/* Le resultat de l'unification est un ensemble de ste */
#endif /* 0 */	

	top = get_new_top_pred_wfs_id ();
	/* wfs_id_pred_list [top].wfs_id = ste; */
	wfs_id_pred_list [top].wfs_id = (*right_val_ptr == ATOM_Aij) ? AIJ : (spf.outputG.maxxnt+PIJ);

	if (left_kind & NEW_FIELD) {
	  /* X est nouveau, Aij ou Pij unique */
	  wfs_id_pred_list [top].next = 0;
	}
	else {
	  wfs_id_pred_list [top].next = (*left_val_ptr >> FS_ID_SHIFT);
	}

	*left_val_ptr = (top << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);
      }

      return SXTRUE;
    }
#endif /* ATOM_Aij || ATOM_Pij */

    /* $$ ... X = "-" et X est bien un champ atomique */
    if (left_kind & NEW_FIELD) {
      /* X est nouveau, on lui affecte right_val */
      if (!checking) {
	*left_val_ptr += (*right_val_ptr << FS_ID_SHIFT);

	/* Le "champ" CONSTRAINT_VAL a ete initialise a 0 (equal_unification) */
	if (is_constrained_unification)
	  *left_val_ptr |= CONSTRAINT_VAL;
      }

      return SXTRUE;
    }

    /* L'unification est l'intersection des valeurs atomiques */
    val = (*left_val_ptr >> FS_ID_SHIFT) & *right_val_ptr;

    if (!checking) {
      if (val) {
	/* On tient compte du champ optionnel */
	*left_val_ptr = (val << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);

	if (!is_constrained_unification)
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;
      }
    }

    return val != 0;
  }

  right_dol = right_kind>>REF_SHIFT;

  /* A FAIRE : $$ path1 = $$ path2 ou tous les 2 sont de type atomique!!
     ou peut etre l'interdire (statiquement) */
  /* Idee : calculer statiquement les types des lhs et rhs d'une unification et les mettre ds le code */

#if EBUG
  if (left_kind>>REF_SHIFT != 0)
    sxtrap (ME, "unify");
#endif /* EBUG */  

  if (right_kind & ATOM_FIELD) {
#if EBUG
    if ((left_kind & ATOM_FIELD) == 0 || right_dol != 0 && (right_kind & NEW_FIELD) != 0)
      sxtrap (ME, "unify");
#endif /* EBUG */  

    left_static_field_kind = field_id2kind [(*left_val_ptr) & FIELD_AND];

#if ATOM_Aij || ATOM_Pij
    if (left_static_field_kind & Aij_KIND) {
      if (!checking)
	/* unbounded_structure_unification marche ds le cas Aij_KIND !! */
	unbounded_structure_unification (left_val_ptr, right_val_ptr, right_dol);

      /* On suppose pas de contraintes (=c) sur les Aij */
      return SXTRUE;
    }
#endif /* ATOM_Aij || ATOM_Pij */

    /* Ici on a $$ {X1}* Y1 = $i {X2}* Y2 et Y1 et Y2 designent des atomes et Y2 est connu,
       sauf peut etre si i == 0 */
    if (right_kind & OLD_FIELD) {
      /* Y2 est connu */
      if (left_kind & OLD_FIELD) {
	/* Y1 est aussi connu */
	/* L'unification est l'intersection des valeurs atomiques */
	if ((*left_val_ptr & FIELD_AND) == PRED_ID) {
	  /* l'unification doit echouer entre 2 pred (meme identiques) */
	  /* Modif le 06/04/04 apre`s discussion avec Lionel :
	     On peut faire l'unification uniquement si (au moins) l'un des pred est contraint
	     l'unification reussie si les 2 lexemes sont identiques et si l'intersection des sous_cat
	     est non vide.  Exception si une sous-cat est vide, elle herite de l'autre.
	     A FAIRE (peut e^tre) prevoir l'extension pred = "lexeme <>" qui ne pourrait s'unifier
	     qu'avec une sous-cat vide et qui serait donc differente de pred = "lexeme" qui pourrait
	     s'unifier avec une sous-cat qqc */
	  if ((*left_val_ptr | *right_val_ptr) & CONSTRAINT_VAL) {
	    /* Au moins l'un des pred est contraint */
	    dpv_lptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, (*left_val_ptr)>>FS_ID_SHIFT)));
	    dpv_rptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, (*right_val_ptr)>>FS_ID_SHIFT)));

	    if (dpv_lptr [0] == dpv_rptr [0]) {
	      if (dpv_rptr [1] == 0 && dpv_rptr [2] == 0) {
		/* left est bon, il faut encore changer le champ contrainte */
		val = (*left_val_ptr) >> FS_ID_SHIFT;
	      }
	      else {
		if ( dpv_lptr [1] == 0 && dpv_rptr [2] == 0) {
		  /* c'est right qui est bon */
		  val = (*right_val_ptr) >> FS_ID_SHIFT;
		}
		else {
		  /* cas general, il faut unifier les sous_cat */
		  /* pour l'instant, on dit que ca echoue (que faire des options, des disjonctions ?) */
		  val = 0;
		}
	      }
	    }
	    else
	      /* lexeme differents => echec */
	      val = 0;
	  }
	  else
	    val = 0;
	}
	else {
	  if (left_static_field_kind & UNBOUNDED_KIND) {
	    /* Cas unbounded normal */
	    val = (*left_val_ptr) >> FS_ID_SHIFT;

	    if (val != ((*right_val_ptr) >> FS_ID_SHIFT))
	      val = 0;
	  }
	  else
	    val = (*left_val_ptr & *right_val_ptr) >> FS_ID_SHIFT;
	}

	if (val && !checking) {
	  *left_val_ptr = (val << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);

	  if (((is_constrained_unification) ? (*left_val_ptr & *right_val_ptr & CONSTRAINT_VAL) : 0) == 0)
	    /* Le resultat doit etre une equal unification */
	    /* RAZ du "champ" CONSTRAINT_VAL */
	    *left_val_ptr &= ~CONSTRAINT_VAL;

	  if (right_dol == 0)
	    *right_val_ptr = (*left_val_ptr & (((~0)<<FS_ID_SHIFT)+CONSTRAINT_VAL))
	      + (*right_val_ptr & (/* OPTIONAL_VAL+ */FIELD_AND));
	}

	return val != 0;
      }

      if (!checking) {
	/* Y1 est nouveau, on lui affecte right_val */
	/* Ds le cas de valeurs atomiques on ne partage pas les champs, on duplique les valeurs !! */
	val = *right_val_ptr >> FS_ID_SHIFT;

	*left_val_ptr = (val << FS_ID_SHIFT) + (*left_val_ptr & FS_ID_AND);

	if (((is_constrained_unification) ? (*right_val_ptr & CONSTRAINT_VAL) : 0))
	  /* Le resultat doit etre une constrained unification */
	  *left_val_ptr |= CONSTRAINT_VAL;


	if (right_dol == 0)
	  *right_val_ptr = (*left_val_ptr & (((~0)<<FS_ID_SHIFT)+CONSTRAINT_VAL))
	    + (*right_val_ptr & (/* OPTIONAL_VAL+ */FIELD_AND));
      }

      return SXTRUE;
    }

    /* La valeur de Y2 est inconnue et $i == $$ */
    /* On est ds le cas $$ {X1}* Y1 = $$ {X2}* Y2 */
    if (left_kind & OLD_FIELD) {
      /* Y1 est connu */
      
      if (!checking) {
	if (!is_constrained_unification)
	  /* Le resultat doit etre une equal unification */
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;

	*right_val_ptr = (*left_val_ptr & (((~0)<<FS_ID_SHIFT)+CONSTRAINT_VAL))
	  + (*right_val_ptr & (/* OPTIONAL_VAL+ */FIELD_AND));
      }

      return SXTRUE;
    }
	
    /* Ici ni Y1 ni Y2 ne sont connus, il faut donc partager les champs */
    if (right_dol == 0)
      /* A FAIRE !! */
      sxtrap (ME, "unify (atom variable, atom variable) is not yet implemented");
    else {
#if EBUG
      printf ("$%i ... %s is not set", right_dol, field_id2string [(*right_val_ptr) & FIELD_AND]);
#endif /* EBUG */ 
      return SXFALSE;
    }
  }

  /* Ici on a des f_structures a gauche et a droite */
  if ((left_kind & DOL_REF) && (right_kind & DOL_REF) && right_dol == 0)
    /* $$ = $$  !! */
    return SXTRUE;

  if (left_kind & UNBOUNDED_STRUCT_FIELD) {
    /* Unification sur des ensembles de f_structures : le resultat est l'union */
    if (checking)
      /* unbounded_structure_unification () marche toujours */
      return SXTRUE;

    if (!unbounded_structure_unification (left_val_ptr, right_val_ptr, right_dol))
      return SXFALSE;
  }
  else {
    if (right_kind & EMPTY_STRUCT) {
      /* $$ X* = [] */
      /* C,a marche toujours :
       Soit X* a ete cree vide
       Soit X* est non vide (et l'unif avec le vide marche */
      return SXTRUE;
    }

    /* $$ X* = $i Y* */
    left_id = *left_val_ptr >> FS_ID_SHIFT;
    right_id = *right_val_ptr >> FS_ID_SHIFT;

    left_id = get_equiv (left_id); /* En fait on est la` !! */

    if (right_dol == 0) {
      /* right_id est ds wfs */
      right_id = get_equiv (right_id); /* En fait on est la` !! */
    }
    else {
      /* right_id est ds fs */ 
      /* Si cette structure est partagee et a deja ete recopiee/unifiee avec un wfs_id, on peut
	 utiliser directement ce wfs_id */
      if (
#if 1
	  /* Le 06/05/04 ESSAI : Les [sous-]structures des $i ne sont instanciees qu'une seule fois ds un bloc d'equations */
	  /* Echec : Voir commentaire ci_dessus ds structure_unification () */
	  XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, right_id+1)-1) < 0 /* shared */ && 
#endif /* 1 */ 
	  (new_wfs_id = shared_fs_id2wfs_id [right_id].rhs [right_dol]) >= 0 /* Pas le 1er coup */) {
	right_id = get_equiv (new_wfs_id);
	right_dol = 0;
      }
    }

    if (!structure_unification (left_id, right_id, right_dol, checking))
      return SXFALSE;
  }
  
  if (!checking) {
    /* Gestion du champ "CONSTRAINT_VAL" */
    if ((left_kind & DOL_REF) == 0) {
      /* $$ X+ = $i Y* */
      if ((right_kind & DOL_REF) == 0) {
	/* $$ X+ = $i Y+ */
	if (((is_constrained_unification) ? (*left_val_ptr & *right_val_ptr & CONSTRAINT_VAL) : 0) == 0)
	  /* Le resultat doit etre une equal unification */
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;
      }
      else {
	/* $$ X+ = $i */
	if (((is_constrained_unification) ? (*left_val_ptr & CONSTRAINT_VAL) : 0) == 0)
	  /* Le resultat doit etre une equal unification */
	  /* RAZ du "champ" CONSTRAINT_VAL */
	  *left_val_ptr &= ~CONSTRAINT_VAL;
      }
    }

    if (right_dol == 0) {
      /* $$ X* = $$ Y* */
      /* right_id devient identique a left_id */
      SXINT old_right_id, x, y;

      if ((right_kind & DOL_REF) == 0) {
	/* $$ X* = $$ Y+ */
	if ((left_kind & DOL_REF) == 0) {
	  /* $$ X+ = $$ Y+ */
	  if (((is_constrained_unification) ? (*left_val_ptr & *right_val_ptr & CONSTRAINT_VAL) : 0) == 0)
	    /* Le resultat doit etre une equal unification */
	    /* RAZ du "champ" CONSTRAINT_VAL */
	    *right_val_ptr &= ~CONSTRAINT_VAL;
	}
	else {
	  /* $$ = $$ Y+ */
	  if (((is_constrained_unification) ? (*right_val_ptr & CONSTRAINT_VAL) : 0) == 0)
	    /* Le resultat doit etre une equal unification */
	    /* RAZ du "champ" CONSTRAINT_VAL */
	    *right_val_ptr &= ~CONSTRAINT_VAL;
	}
      }
    }
  }

  return SXTRUE;
}



static SXBOOLEAN
call_unify (operator, left_val_ptr, left_kind, right_val_ptr, right_kind, checking)
     SXINT     operator, *left_val_ptr, left_kind, *right_val_ptr, right_kind;
     SXBOOLEAN checking;
{
  SXINT              dum_kind, x, id;
  SXINT              *dum_val_ptr; 
  SXBOOLEAN          ret_val;

  switch (operator) {
  case OPERATOR_UNIFY:
  case OPERATOR_CONSTRAINT:
    if (operator == OPERATOR_CONSTRAINT && left_kind>>REF_SHIFT && (right_kind & (ATOM_VAL))) {
      /* On permet du
	 $i ... v_form =c infinitive
      */
#if EBUG
      /* A FAIRE statiquement */
      if (left_kind & DOL_REF)
	sxtrap (ME, "call_unify");
#endif /* EBUG */

      is_constrained_unification = SXTRUE;
      ret_val = unify (left_val_ptr, left_kind, right_val_ptr, right_kind, SXTRUE); /* checking */

      break;
    }
      
    /* La partie gauche doit referencer $$ */
    if (left_kind>>REF_SHIFT) {
      /* On a $i en lhs => on swap */
      dum_val_ptr = left_val_ptr;
      dum_kind = left_kind;
      left_val_ptr = right_val_ptr;
      left_kind = right_kind;
      right_val_ptr = dum_val_ptr;
      right_kind = dum_kind;
    }

#if EBUG
    if ((left_kind & (ATOM_VAL)) || (left_kind>>REF_SHIFT != 0))
      sxtrap (ME, "call_unify");

    /* A FAIRE statiquement */
    if (operator == OPERATOR_CONSTRAINT && (left_kind & DOL_REF))
      sxtrap (ME, "call_unify");
#endif /* EBUG */

    is_constrained_unification = (operator == OPERATOR_CONSTRAINT);

    
    ret_val = unify (left_val_ptr, left_kind, right_val_ptr, right_kind, checking);

    break;

  case OPERATOR_IN:
    /* La partie droite doit referencer $$ ... */
#if EBUG
    if (right_kind>>REF_SHIFT)
      sxtrap (ME, "call_unify (OPERATOR_IN)");
#endif /* EBUG */

    ret_val = unify_in (left_val_ptr, left_kind, right_val_ptr, right_kind);
    /* ret_val == SXTRUE : unify_in marche toujours */

    break;

  case OPERATOR_POSSIBLE:
  default:
    sxtrap (ME, "call_unify");
  }

  return ret_val;
}

static SXINT call_tfs_instantiation ();

/* On essaie d'evaluer l'equation dont le code est repere par xcode sur "prod" */
static SXBOOLEAN
evaluate (xcode)
     SXINT xcode;
{
  SXINT              left_val, right_val, left_kind, right_kind, eval_result, operator, lhs_code_size;
  SXINT              *right_val_ptr, *left_val_ptr; 
  SXINT              *store_left_pcs, *store_right_pcs;
  SXINT              left_eval_result, right_eval_result, x, y, l;
  SXINT              prev_post_stack_top, nb, post_stack_kind;
  SXBOOLEAN          ret_val, is_left_re, is_right_re, is_operator_possible, is_skip, done;
  struct re_result *re_result_left_ptr, *re_result_right_ptr;

  is_skip = SXFALSE;
#if EBUG
  print_equation_code (xcode);
#endif /* EBUG */

  pcs = code_stack+xcode;
  operator = *pcs;

  while (operator == OPERATOR_ETIQUETTE) {
    left_val_ptr = &left_val; /* finesse */

    if ((eval_result = eval (&left_val_ptr, &left_kind)) <= 0) {
#if EBUG
      printf (" ... $%i is empty : skip\n", left_kind>>REF_SHIFT);
#endif /* EBUG */
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
    /* Attention :
       Si un calcul de chemin echoue parcequ'un $i reference un range vide, l'equation ne doit
       produire aucun effet de bord et on doit executer l'equation suivante 
       Ex : $$ a b ($i pcas)-obj = ... Ne pas creer a b si $i est vide
       $$ obj = $i Ne pas creer obj si $i est vide */

    lhs_code_size = *(--pcs);

    if (is_left_re = (pcs [-1] == OPERATOR_RE)) {
      pcs--;
      lhs_code_size--;
    }

    store_left_pcs = pcs;
    left_eval_result = re_eval (0);

    if (left_eval_result == -1) {
      is_skip = ret_val = SXTRUE;
      break;
    }

    if (left_eval_result == 0) {
      /* Attention pcs n'est pas bien positionne ... */
      /* ... on le fait */
      pcs = store_left_pcs-lhs_code_size;
    }

    if (is_right_re = (pcs [-1] == OPERATOR_RE))
      pcs--;

    store_right_pcs = pcs;
    right_eval_result = re_eval (0);

    if (right_eval_result == -1) {
      is_skip = ret_val = SXTRUE;
      break;
    }

    if (is_left_re) {
      if (left_eval_result == 0) {
	is_skip = ret_val = SXFALSE;
	break;
      }

      /* La liste des resultats se trouve en re_results [left_eval_result] */
    }
    else {
      if (left_eval_result == 0) {
	/* Ca necessite de la creation de champs, on le fait */
	pcs = store_left_pcs;
	left_val_ptr = &left_val; /* finesse */

	if ((eval_result = eval (&left_val_ptr, &left_kind)) <= 0) {
	  /* Normalement, ici,  eval_result >= 0 */
	  is_skip = ret_val = (eval_result != 0);
	  /* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	  break;
	}
      }
      else {
	re_result_left_ptr = re_results+left_eval_result;
	/* Normalement re_result_left_ptr->next == 0 */
	/* Un seul resultat => cas standard */
	left_val_ptr = re_result_left_ptr->ptr;
	left_kind = re_result_left_ptr->kind;
      }
    }

    if (is_left_re) {
      re_result_left_ptr = re_results+left_eval_result;

      if (re_result_left_ptr->next == 0) {
	/* Un seul resultat => cas standard */
	left_val_ptr = re_result_left_ptr->ptr;
	left_kind = re_result_left_ptr->kind;
	x = left_eval_result;
	left_eval_result = 0;
      }
    }
    else
      x = left_eval_result = 0;

    prev_post_stack_top = DTOP (post_stack);
    done = SXFALSE;
      
#if IS_OPERATOR_LEX_REF
    /* L'operateur OPERATOR_LEX_REF est ds le code genere pour la LFG */
    /* Les lexeme_id2display et args_ref_list de l'include lex_lfg_h sont visibles ... */
    if (store_right_pcs [-1] == OPERATOR_LEX_REF) {
      SXINT  right_dol, fs_id, bot, top, field_id, val, lexeme_id, struct_id, right_val;
      SXINT  *right_attr_ptr;

#if EBUG
      if (is_right_re || right_eval_result == 0)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

      re_result_right_ptr = re_results+right_eval_result;
      /* Normalement re_result_right_ptr->next == 0 */
      /* la syntaxe ne permet que ... = lex $i */
      /* Un seul resultat => cas standard */
      right_val_ptr = re_result_right_ptr->ptr;
      right_kind = re_result_right_ptr->kind;

      right_dol = right_kind>>REF_SHIFT;

#if EBUG
      if (right_dol == 0)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

      /* $$ X* = lex $right_dol */
      /* Si $right_dol n'a pas de champ pred => echec de l'unification */
      fs_id = *right_val_ptr >> FS_ID_SHIFT;

      if (fs_id) {
	bot = XH_X (XH_fs_hd, fs_id);
	right_attr_ptr = &(XH_list_elem (XH_fs_hd, bot));
	field_id =  *right_attr_ptr & FIELD_AND;
      }
      else
	field_id == 0;

      if (field_id != PRED_ID) {
	/* Il n'y a pas de champ pred => echec de l'unification */
	is_skip = ret_val = SXFALSE;
	break;
      }

      val = (*right_attr_ptr)>>FS_ID_SHIFT;
      lexeme_id = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val));

#if EBUG
      if (lexeme_id == 0)
	sxtrap (ME, "evaluate");
#endif /* EBUG */ 

      right_kind = (right_dol<<REF_SHIFT)+STRUCT_FIELD;

      /* On parcourt la liste des structures lexicales dont le lexeme a l'identifiant lexeme_id */
      bot = lexeme_id2display [lexeme_id];
      top = lexeme_id2display [lexeme_id+1];

      /* On parcourt la liste des structures lexicales dont le lexeme a l'identifiant lexeme_id */
      while (bot < top) {
	/* On est ds le cas de disjonctions */
	/* Appel avec Tpq == 0 => les fs_id trouves sont nouveaux ... */
	fs_id =  call_tfs_instantiation (args_ref_list [bot], 0 /* Tpq */);
#if EBUG
	print_f_structure (0 /* pas de Xpq !! */, fs_id, SXFALSE);
#endif /* EBUG */

	right_val = fs_id <<FS_ID_SHIFT;

	if (left_eval_result == 0) {
	  /* Une reference unique en LHS */
	  /* On cherche les fs_id pour lesquels l'unif marche */
	  if (call_unify (operator, left_val_ptr, left_kind, &right_val, right_kind, SXTRUE) ) {
	    /* unifiable */
	    DCHECK (post_stack, 5);
	    DSPUSH (post_stack, right_kind);
	    DSPUSH (post_stack, right_val);

	    if (x == 0) {
	      /* is_left_re == false => un seul resultat que l'on doit mettre ds re_results pour
	       le post_evaluate () eventuel */
	      x = re_results_get_new_top ();
	      re_result_left_ptr = re_results+x;
	      re_result_left_ptr->ptr = left_val_ptr;
	      re_result_left_ptr->kind = left_kind;
	      re_result_left_ptr->next = 0;
	    }

	    DSPUSH (post_stack, x);
	    DSPUSH (post_stack, operator);
	    DSPUSH (post_stack, POST_STACK_fs_id_x);
	  }
#if EBUG
	  else
	    fputs ("Unification failed\n", stdout);
#endif /* EBUG */
	}
	else {
	  for (x = left_eval_result; x != 0; x = re_results [x].next) {
	    re_result_left_ptr = re_results+x;
	    left_val_ptr = re_result_left_ptr->ptr;
	    left_kind = re_result_left_ptr->kind;

	    /* Sur le dernier couple x y, on ne fait pas le test avant d'unifier */
	    if (call_unify (operator, left_val_ptr, left_kind, &right_val, right_kind, SXTRUE) ) {
	      /* Pour l'instant, on conserve le 1er qui marche ... */
	      DCHECK (post_stack, 4);
	      DSPUSH (post_stack, right_val);
	      DSPUSH (post_stack, x);
	      DSPUSH (post_stack, operator);
	      DSPUSH (post_stack, POST_STACK_fs_id_x);
	    }
	  }
	}

	bot++;
      }
    }
    else 
#endif /* IS_OPERATOR_LEX_REF */
      {
      if (is_right_re) {
	if (right_eval_result == 0) {
	  is_skip = ret_val = SXFALSE;
	  break;
	}

	/* La liste des resultats se trouve en re_results [right_eval_result] */
      }
      else {
	if (right_eval_result == 0) {
	  /* Ca necessite de la creation de champs, on le fait */
	  pcs = store_right_pcs;
	  right_val_ptr = &right_val; /* finesse */

	  if ((eval_result = eval (&right_val_ptr, &right_kind)) <= 0) {
	    /* Normalement, ici,  eval_result >= 0 */
	    is_skip = ret_val = (eval_result != 0);
	    /* Si $i < $$ X+ et $i est vide on ne cree pas X+ ds $$ */
	    break;
	  }
	}
	else {
	  re_result_right_ptr = re_results+right_eval_result;
	  /* Normalement re_result_right_ptr->next == 0 */
	  /* Un seul resultat => cas standard */
	  right_val_ptr = re_result_right_ptr->ptr;
	  right_kind = re_result_right_ptr->kind;
	}
      }

      if (is_right_re) {
	re_result_right_ptr = re_results+right_eval_result;

	if (re_result_right_ptr->next == 0) {
	  /* Un seul resultat => cas standard */
	  right_val_ptr = re_result_right_ptr->ptr;
	  right_kind = re_result_right_ptr->kind;
	  right_eval_result = 0;
	}
      }
      else
	right_eval_result = 0;

      if (left_eval_result == 0 && right_eval_result == 0) {
	/* On ne verifie pas que l'unif marche avant de la faire */
	ret_val = call_unify (operator, left_val_ptr, left_kind, right_val_ptr, right_kind, SXFALSE);
	done = SXTRUE;
      }
      else {
	for (x = left_eval_result; x != 0; x = re_results [x].next) {
	  re_result_left_ptr = re_results+x;
	  left_val_ptr = re_result_left_ptr->ptr;
	  left_kind = re_result_left_ptr->kind;
      
	  for (y = right_eval_result; y != 0; y = re_results [y].next) {
	    re_result_right_ptr = re_results+y;
	    right_val_ptr = re_result_right_ptr->ptr;
	    right_kind = re_result_right_ptr->kind;

	    if (call_unify (operator, left_val_ptr, left_kind, right_val_ptr, right_kind, SXTRUE)) {
	      DCHECK (post_stack, 4);
	      DSPUSH (post_stack, x);
	      DSPUSH (post_stack, y);
	      DSPUSH (post_stack, operator);
	      DSPUSH (post_stack, POST_STACK_x_y);
	    }
	  }
	}
      }
    }
	
    if (!done) {
      if (nb = DTOP (post_stack)-prev_post_stack_top) {
	/* non vide */
	if (nb == 4 || nb == 5) {
	  /* Unification unique */
	  post_stack_kind = DPOP (post_stack);
	  operator = DPOP (post_stack);

	  if (post_stack_kind == POST_STACK_x_y) {
	    y = DPOP (post_stack);
	    re_result_right_ptr = re_results+y;
	    right_val_ptr = re_result_right_ptr->ptr;
	    right_kind = re_result_right_ptr->kind;
	    x = DPOP (post_stack);
	    re_result_left_ptr = re_results+x;
	    left_val_ptr = re_result_left_ptr->ptr;
	    left_kind = re_result_left_ptr->kind;
		
	    if (!call_unify (operator, left_val_ptr, left_kind, right_val_ptr, right_kind, SXFALSE))
	      /* Il doit marcher ... */
	      sxtrap (ME, "evaluate");
	  }
	  else {
	    /* References multiples en LHS */
	    /* post_stack_kind == POST_STACK_fs_id_x */
	    x = DPOP (post_stack);
	    re_result_left_ptr = re_results+x;
	    left_val_ptr = re_result_left_ptr->ptr;
	    left_kind = re_result_left_ptr->kind;
	    right_val = DPOP (post_stack);
	    right_kind = DPOP (post_stack);

	    if (!call_unify (operator, left_val_ptr, left_kind, &right_val, right_kind, SXFALSE))
	      /* ... elle doit marcher */
	      sxtrap (ME, "evaluate");
	  }
	}
	else {
	  /* Ici plusieurs unif peuvent marcher */
	  DPUSH (post_display, prev_post_stack_top);
	}
	    
	ret_val = SXTRUE;
      }
      else {
	/* Echec de l'operateur lex */
	is_skip = ret_val = SXFALSE;
      }
    }

    break;

  case OPERATOR_NONEXIST:
  case OPERATOR_EXIST:
    if (pcs [-1] == OPERATOR_RE)
      /* On le saute ... */
      pcs--;

    left_eval_result = re_eval (0);

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
#if EBUG
  printf (" ... %s\n", ret_val ? (is_skip ? "skip" : "true") : "false");
#endif /* EBUG */

  return ret_val;
}

/* On vient de terminer l'evaluation d'un bloc d'equations dont le resultat est ds wfs
   mais certaines equations correspondent a des disjonctions multiples (expressions regulieres, operateur lex)
   n'ont pas ete instanciees, on le fait */
/* On traite le "niveau" cur_post */
static SXBOOLEAN
post_evaluate (Aij, cur_post)
     SXINT Aij, cur_post;
{
  SXINT              x, y, xstore, fs_id, head;
  SXINT              left_kind, right_kind, right_val, post_stack_kind, operator;
  SXINT              top_post, cur, top;
  SXINT              *right_val_ptr, *left_val_ptr;
  struct re_result *re_result_left_ptr, *re_result_right_ptr;
  SXBOOLEAN          done, local_call, ret_val;

  if (cur_post > (top_post = DTOP (post_display))) {
    /* fin d'un spine */
    if (ret_val = coherence (Aij)) {
      fs_id = fill_fs ();
      head = fill_heads (Aij, fs_id);

      if (is_locally_unconsistent)
	SXBA_1_bit (unconsistent_heads_set, head);

      SXBA_1_bit (Xpq_set, Aij);
#if EBUG
      print_f_structure (Aij, fs_id, is_locally_unconsistent);
#endif /* EBUG */

      if (is_forest_filtering || xml_file) {
	DPUSH (fs_id_Pij_dstack, head);
	DPUSH (fs_id_Pij_dstack, doldol);
	fs_id_Pij_dstack [Pij_cur_bot] += 2;
      }
    }
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
	re_result_right_ptr = re_results+y;
	right_val_ptr = re_result_right_ptr->ptr;
	right_kind = re_result_right_ptr->kind;
	x = post_stack [top--];
	re_result_left_ptr = re_results+x;
	left_val_ptr = re_result_left_ptr->ptr;
	left_kind = re_result_left_ptr->kind;
		
	local_call = call_unify (operator, left_val_ptr, left_kind, right_val_ptr, right_kind, SXFALSE);
      }
      else {
	/* References multiples en LHS */
	/* post_stack_kind == POST_STACK_fs_id_x */
	x = post_stack [top--];
	re_result_left_ptr = re_results+x;
	left_val_ptr = re_result_left_ptr->ptr;
	left_kind = re_result_left_ptr->kind;
	right_val = post_stack [top--];
	right_kind = post_stack [top--];

	local_call = call_unify (operator, left_val_ptr, left_kind, &right_val, right_kind, SXFALSE);
      }

      if (local_call) {
	if (post_evaluate (Aij, cur_post+1)) {
	  ret_val = SXTRUE;
	}
      }

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
     SXINT Aij;
{
  SXBOOLEAN ret_val;
  ret_val = post_evaluate (Aij, 1);

#if EBUG
  printf ("%s of the post evaluation\n", ret_val ? "Success" : "Failure");
#endif /* EBUG */

  clear_wfs ();
  DRAZ (post_display);
  DRAZ (post_stack);

  return ret_val;
}


static void
set_failure_set (pos, n)
     SXINT pos, n;
{
  SXINT Xpq, fs_id_nb, i;

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
      set_failure_set (pos-1, n+rhs_pos2digit [pos]);
    else {
      for (i = 0; i < fs_id_nb; i++) {
	set_failure_set (pos-1, n+i);
      }
    }
  }
}


/* Cas UNFOLD */
static void
clear_Xpq (Xpq)
     SXINT Xpq;
{
  SXINT d, pair;
  SXINT *base_ptr;

  base_ptr = fs_id_dstack + Xpq2disp [Xpq];
  d = *base_ptr++;
  Xpq2disp [Xpq] = 0;

  while (d--) {
    pair = *base_ptr++;
    XxY_erase (heads, pair);
  }
}

static void
clear_Xpq_set ()
{
  SXINT Xpq;

  Xpq= -1;

  while ((Xpq= sxba_scan_reset (Xpq_set, Xpq)) >= 0)
    clear_Xpq (Xpq);

  sxba_empty (unconsistent_heads_set);
}

static void
clear_for_relaxed_run ()
{
  SXINT *p;

  if (is_forest_filtering || xml_file) {
    p = Pij2disp+spf.outputG.maxprod;

    do {
      *p = 0;
    } while (--p >= Pij2disp);

    DRAZ (fs_id_Pij_dstack);
  }

  clear_Xpq_set ();
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
static SXBOOLEAN
tree_walk (prod)
     SXINT prod;
{
  SXINT        rule, equation_block, xcode, struct_nb, rhs_pos, pos, q, d, Xpq, init_prod, head, x, fs_id, mod, cur_bot;
  SXINT        *ptr, *base_ptr, *bot_ptr;
  SXBOOLEAN    ret_val;
  struct lhs *plhs;
  struct rhs *p;

#if EBUG
  /* On imprime cette production... */
  spf_print_prod (stdout, prod, SXTRUE /* with nl */);
#endif /* EBUG */

  /* Aij et prhs sont statiques... */ 
  plhs = spf.outputG.lhs+prod;
  Aij = plhs->lhs; /* $$ */

  if (Aij2i (Aij) == Aij2j (Aij)) {
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
    }

#if EBUG
    print_f_structure (Aij, 0, SXFALSE);
#endif /* EBUG */

    return SXTRUE;
  }

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

  ret_val = SXFALSE;

  if (struct_nb) {
    if (is_forest_filtering || xml_file) {
      bot_ptr = Pij2disp+prod;

#if EBUG
      if (*bot_ptr != 0)
	sxtrap (ME, "tree_walk");
#endif /* EBUG */

      cur_bot = DTOP (fs_id_Pij_dstack);
    }

#if ATOM_Aij
    AIJ = Aij; /* Pour unify () */
#endif /* ATOM_Aij */
#if ATOM_Pij
    PIJ = prod; /* Pour unify () */
#endif /* ATOM_Pij */

    for (rule = prod2rule [init_prod]; rule != 0; rule = rule2next_rule [rule]) {
#if EBUG
      printf ("lfg_rule #%i:\n", rule);
#endif /* EBUG */

      equation_block = rule2equation [rule];

      if (equation_block == 0) {
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

	  DPUSH (fs_id_dstack, 0);
	  fs_id_dstack [x]++; /* et un fs_id de plus ... */

#if EBUG
	  print_f_structure (Aij, 0, SXFALSE);
#endif /* EBUG */
	}

	if (is_forest_filtering || xml_file) {
	  DPUSH (fs_id_Pij_dstack, 2);
	  DPUSH (fs_id_Pij_dstack, head);
	  DPUSH (fs_id_Pij_dstack, -1); /* Cas particulier */
	}
      }
      else {
	/* Pour chaque combinaison des f_structures de la rhs on calcule le $$ correspondant... */
	if (failure_set) {
	  sxba_empty (failure_set);
		    
	  if (SXBASIZE (failure_set) < struct_nb)
	    failure_set = sxba_resize (failure_set, struct_nb);
	}

	if (is_forest_filtering || xml_file) {
	  DPUSH (fs_id_Pij_dstack, 0);
	  Pij_cur_bot = DTOP (fs_id_Pij_dstack);
	}

	for (doldol = 0; doldol < struct_nb; doldol++) {
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
	      rhs_pos2fs [pos] = base_ptr [mod];

#if EBUG
	      if (XxY_X (heads, rhs_pos2fs [pos]) != Xpq)
		sxtrap (ME, "tree_walk");
#endif /* EBUG */

	      q /= d;
	    }

	    /* Tous les "head" de la RHS existent */
	    ptr = equation_stack+equation_block;

	    if ((xcode = *ptr++) > 0 && code_stack [xcode] == OPERATOR_IDENTITY) {
	      /* On est ds le cas particulier ou la seule equation du bloc est $$ = $1.
		 On se contente donc de recopier le fs_id de la RHS */
	      ret_val = SXTRUE;
	      fs_id = XxY_Y (heads, rhs_pos2fs [1]);
	      head = fill_heads (Aij, fs_id);
	      SXBA_1_bit (Xpq_set, Aij);
#if EBUG
	      print_f_structure (Aij, fs_id, SXFALSE);
#endif /* EBUG */

	      if (is_forest_filtering || xml_file) {
		DPUSH (fs_id_Pij_dstack, head);
		DPUSH (fs_id_Pij_dstack, doldol);
		fs_id_Pij_dstack [Pij_cur_bot] += 2;
	      }
	    }
	    else {
	      wfs_id = 0;
	      wfs_id2equiv [0] = 0;
	      wfs2attr [0] [0] [0] = 0;
	      wfs_id_pred_list [0].wfs_id = 0; /* top */
	      sxba_empty (doli_used_set);

	      /* Clear de re_results */
	      re_results [0].kind = 0;
	      DRAZ (re_tank);

	      while (shared_fs_id_stack [0]) {
		pos = DPOP (shared_fs_id_stack);
		shared_fs_id2wfs_id [DPOP (shared_fs_id_stack)].rhs [pos] = -1;
	      }

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
		  if (coherence (Aij)) {
		    ret_val = SXTRUE;
		    fs_id = fill_fs ();
		    head = fill_heads (Aij, fs_id);

		    if (is_locally_unconsistent)
		      SXBA_1_bit (unconsistent_heads_set, head);

		    SXBA_1_bit (Xpq_set, Aij);
#if EBUG
		    print_f_structure (Aij, fs_id, is_locally_unconsistent);
#endif /* EBUG */

		    if (is_forest_filtering || xml_file) {
		      DPUSH (fs_id_Pij_dstack, head);
		      DPUSH (fs_id_Pij_dstack, doldol);
		      fs_id_Pij_dstack [Pij_cur_bot] += 2;
		    }
		  }
		  else {
		    /* Test de coherence a e'choue' */
		    clear_wfs ();
		    DRAZ (post_display);
		    DRAZ (post_stack);
		    head = 0;
		  }
		}
	      }
	      else {
		/* Echec */
		/* Il faut razer wfs */
		clear_wfs ();
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

		  set_failure_set (rhs_pos, 0);
		}
	      }
	    }
	  }
#if EBUG
	  printf ("  Occur #%i/%i: (%s)\n", doldol+1, struct_nb, (head != 0) ? "success" : "failure");
#endif /* EBUG */
	}
      }
    }

    if (is_forest_filtering || xml_file) {
      if (ret_val)
	/* Non vide => on note */
	*bot_ptr = cur_bot+1;
      else
	/* On remet fs_id_Pij_dstack en etat */
	fs_id_Pij_dstack [0] = cur_bot;
    }
  }
#if EBUG
  else {
    printf ("No subtrees for production (cfg=#%i, forest=#%i) mapping the lfg_rules: ", init_prod, prod);

    for (rule = prod2rule [init_prod]; rule != 0; rule = rule2next_rule [rule]) {
      printf ("#%i ", rule);
    }

    fputs ("\n", stdout);
  }
#endif /* EBUG */

  /* if (ret_val) => au moins une des f_structures associee a la LHS de prod est correcte */
  return ret_val;
}


#if EBUG
static void
print_static_structure (root_call, struct_id, lexeme_id)
     SXBOOLEAN root_call;
     SXINT     struct_id, lexeme_id;
{
  SXINT             bot2, cur2, top2, bot3, cur3, top3;
  SXINT             field_id, field_kind, sous_cat_id, atom_id, local_atom_id;
  SXINT             *local2atom_id;
  SXBOOLEAN         is_optional, is_first, is_first2, is_first3;
  struct attr_val *ptr;
  struct pred_val *ptr2;
  unsigned char   static_field_kind;

  fputs ("[", stdout);

  is_first = SXTRUE;

  while (struct_id) {
    ptr = attr_vals+struct_id;
    field_id = ptr->attr;
    static_field_kind = field_id2kind [field_id];
    field_kind = ptr->kind;
    struct_id = ptr->val;

    if (struct_id < 0)
      /* nommee */
      break;

    if (is_first)
      is_first = SXFALSE;
    else
      fputs (", ", stdout);


    if (field_id == PRED_ID) {
      if (static_field_kind != ATOM_KIND)
	sxtrap (ME, "print_static_structure");

      ptr2 = pred_vals+struct_id;
      
      fputs ("pred =", stdout);

      if (field_kind & ASSIGN_CONSTRAINT_KIND)
	fputs ("c", stdout);
      
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

      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND)
	fputs ("= {", stdout);
      else {
	if (field_kind & ASSIGN_CONSTRAINT_KIND)
	  fputs ("=c ", stdout);
	else
	  fputs ("= ", stdout);
      }

      if (static_field_kind & ATOM_KIND) {
	if (static_field_kind & UNBOUNDED_KIND)
	  printf ("%s", atom_id2string [struct_id]);
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

	      printf ("%s", atom_id2string [atom_id]);
	    }
	  }
	}
      }
      else {
	if (static_field_kind == STRUCT_KIND) {
	  print_static_structure (SXFALSE, struct_id, 0);
	}
      }

      if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND)
	fputs ("}", stdout);
    }
    
    struct_id = ptr->next;
  }
      
  fputs ("]", stdout);

  if (struct_id < 0)
    /* nommee */
    printf ("%i", -struct_id);

  if (root_call)
    fputs ("\n", stdout);
}
#endif /* EBUG */

static SXINT
simulate_tfs_instantiation (fs_id, struct_id)
     SXINT fs_id, struct_id;
{
  SXINT             dl_id;  
  struct attr_val *ptr;

  do {
    ptr = attr_vals+struct_id;

    if (field_id2kind [ptr->attr] == STRUCT_KIND) {
      if (!XxY_set (&dl_hd, ptr->val, 0, &dl_id)) {
	/* Le triplet (struct_id, 0, 0) est nouveau */
	/* on simule le calcul de son fs_id */
#if EBUG
	dl_id2fs_id [dl_id] = 0; /* Pour detecter les cycles */
#endif /* EBUG */
	dl_id2fs_id [dl_id] = -(fs_id = simulate_tfs_instantiation (fs_id, ptr->val));
      }
#if EBUG
      else {
	if (dl_id2fs_id [dl_id] < 0)
	  sxtrap (ME, "simulate_tfs_instantiation");
	/* else (dl_id2fs_id [dl_id] == 0) detection d'un cycle 
	   ou (dl_id2fs_id [dl_id] > 0) deja instancie */
      }
#endif /* EBUG */
    }
  } while (struct_id = ptr->next);

  return fs_id;
}

/* On "recopie" la structure lexicale (struct_id, lexeme_id, priority) ds fs_id */
static SXINT
tfs_instantiation (struct_id, lexeme_id, priority, Tpq)
     SXINT  struct_id, lexeme_id, priority, Tpq;
{
  SXINT             dl_id, field_id, field_kind, field_val, fs_id, index;
  struct attr_val *ptr;
  struct pred_val *pv_ptr;
  unsigned char   static_field_kind;
  
  if (field_val = struct_id) {
    /* Structure non vide */
    /* 1er passage */
    do {
      ptr = attr_vals+field_val;

      if (field_id2kind [ptr->attr] == STRUCT_KIND) {
	if (!XxY_set (&dl_hd, ptr->val, 0, &dl_id)) {
	  /* Le triplet (ptr->val, 0, 0) est nouveau */
	  /* on l'instancie */
	  dl_id2fs_id [dl_id] = tfs_instantiation (ptr->val, 0, 0, 0);
	}
	else {
	  if ((fs_id = dl_id2fs_id [dl_id]) < 0) {
	    if (tfs_instantiation (ptr->val, 0, 0, 0) != -fs_id)
	      sxtrap (ME, "tfs_instantiation");

	    dl_id2fs_id [dl_id] = -fs_id;
	  }
#if EBUG
	  else {
	    if (fs_id == 0)
	      sxtrap (ME, "tfs_instantiation");

	    /* deja instancie' */
	  }
#endif /* EBUG */
	}
      }
    } while (field_val = ptr->next);

    /* Ici toutes les sous-structures de struct_id ont un numero > 0, on peut donc generer struct_id */
    field_val = struct_id;

    do {
      ptr = attr_vals+field_val;
      field_val = ptr->val;

      if (field_val < 0) {
	/* Structure partagee */
#if EBUG
	if (ptr->next != 0)
	  /* Ce doit etre la derniere */
	  sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */
	break;
      }

      field_id = ptr->attr;
      static_field_kind = field_id2kind [field_id];
      field_kind = ptr->kind;

      if (field_id == PRED_ID) {
#if EBUG
	if (static_field_kind != ATOM_KIND)
	  sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	/* On stocke les attributs des pred ds dynam_pred_vals et c'est l'index ds dynam_pred_vals
	   qui est stocke ds attr_ptr. */
	/* On y met aussi la priority !! */
	if (lexeme_id == 0)
	  lexeme_id = pred_vals [field_val].lexeme;

	pv_ptr = pred_vals+field_val;
	XH_push (dynam_pred_vals, lexeme_id);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat1);
	XH_push (dynam_pred_vals, pv_ptr->sous_cat2);
	XH_push (dynam_pred_vals, priority);
	XH_push (dynam_pred_vals, Tpq); /* Le 14/06/04 pour rank_by_vicinity () */

	XH_set (&dynam_pred_vals, &field_val);
      }
      else {
	if (static_field_kind == STRUCT_KIND) {
	  if (index = (field_kind >> STRUCTURE_NAME_shift)) {
	    /* structure nommee */
	    /* on lui fait correspondre la structure nommee de meme nom si elle existe */
	    field_val = index2struct_id [index];

#if EBUG
	    if (field_val <= 0)
	      sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	    field_kind &= ASSIGN_CONSTRAINT_KIND; /* On enleve l'index */
	  }

	  dl_id = XxY_is_set (&dl_hd, field_val, 0);
#if EBUG
	  if (dl_id == 0)
	    sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */

	  field_val = dl_id2fs_id [dl_id];

#if EBUG
	  if (field_val <= 0)
	    sxtrap (ME, "tfs_instantiation");
#endif /* EBUG */
	}
	/* else field_val est OK */
      }
      
      field_val <<= FS_ID_SHIFT;
      field_val += (field_kind << MAX_FIELD_ID_SHIFT) + field_id;
      XH_push (XH_fs_hd, field_val);
    } while (field_val = ptr->next);
      
    if (field_val < 0) {
      /* Structure nommee partagee (vide ou non) ... */
      shared_struct_nb++;
      XH_push (XH_fs_hd, -shared_struct_nb); /* Unique ds $$ */
    }
  }

  if (!XH_set (&XH_fs_hd, &fs_id)) {
    /* nouveau */
    shared_fs_id2wfs_id [fs_id] = empty_shared_rhs;
  }

  return fs_id;
}

static void
process_named_structures (struct_id)
     SXINT struct_id;
{
  SXINT             field_kind, index;
  struct attr_val *ptr;

  do {
    ptr = attr_vals+struct_id;

    if (field_id2kind [ptr->attr] == STRUCT_KIND) {
      field_kind = ptr->kind;
      index = field_kind >> STRUCTURE_NAME_shift;

      if (index) {
	/* reference a une structure partagee nommee */
	SXBA_1_bit (index_set, index);
	struct_id = ptr->val;

	if (index2struct_id [index] == 0)
	  index2struct_id [index] = struct_id;
	else {
	  if (attr_vals [struct_id].val >= 0) {
	    /* Cette structure indexee est non vide */
	    index2struct_id [index] = struct_id;
	  }
	}
      }

      process_named_structures (ptr->val);
    }
  } while (struct_id = ptr->next);
}

static SXINT
call_tfs_instantiation (x, Tpq)
     SXINT  x, Tpq;
{
  SXINT                        struct_id, lexeme_id, priority, dl_id, fs_id, index, lex_id;
  struct structure_args_list *ptr2;

  ptr2 = structure_args_list+x;

  lexeme_id = ptr2->lexeme;
  struct_id = ptr2->struct_id & X3F; /* On enleve is_cyclic ou is_named eventuels */
  /* Si struct_id == 0, on a la definition d'une forme flechie sans structure comme :
     '.' poncts; */
  priority = ptr2->priority;

#if EBUG
  print_static_structure (SXTRUE, struct_id, lexeme_id);
#endif /* EBUG */

  if (priority) {
    XxY_set (&prio_hd, lexeme_id, priority, &dl_id);
    lex_id = -dl_id;
    /* lexeme_id < 0 => y'a de la priorite */
  }
  else {
    if (lexeme_id+struct_id == 0)
      /* La structure (toute) vide a pour fs_id zero */
      return 0;

    lex_id = lexeme_id;
  }

  if (XxY_set (&dl_hd, struct_id, lex_id, &dl_id)) {
    /* Le triplet (struct_id, lexeme_id, priority) a deja ete instancie' */
#if EBUG
    fs_id = dl_id2fs_id [dl_id];

    if (fs_id > 0)
      /* OK */
      return fs_id;

    sxtrap (ME, "call_tfs_instantiation");
#else
    return dl_id2fs_id [dl_id];
#endif /* EBUG */
  }
  
#if EBUG
  dl_id2fs_id [dl_id] = 0;
#endif /* EBUG */

  if (ptr2->struct_id & X40) {
    /* La structure struct_id a des structures partagees nommees */
    /* Les structures vides nommees doivent s'instancier vers la structure non vide de meme nom
       si elle existe */
    shared_struct_nb = 0;
    process_named_structures (struct_id);

    if (ptr2->struct_id & X80) {
      /* La structure struct_id est cyclique */
      fs_id = simulate_tfs_instantiation (XH_top (XH_fs_hd), struct_id);
      /* Les nouveaux fs_id sont en negatifs */
      dl_id2fs_id [dl_id] = tfs_instantiation (struct_id, lexeme_id, priority, Tpq);

#if EBUG
      if (dl_id2fs_id [dl_id] != fs_id)
	sxtrap (ME, "call_tfs_instantiation");
#endif /* EBUG */ 
    }
    else
      dl_id2fs_id [dl_id] = tfs_instantiation (struct_id, lexeme_id, priority, Tpq);

    index = 0;

    while ((index = sxba_scan_reset (index_set, index)) > 0)
      index2struct_id [index] = 0;
  }
  else
    dl_id2fs_id [dl_id] = tfs_instantiation (struct_id, lexeme_id, priority, Tpq);

  return dl_id2fs_id [dl_id];
}



/* On instancie le terminal Xpq de la forme flechie if_id */
static void
terminal_instantiation (Xpq, X, if_id/* , terminal_string */)
     SXINT  Xpq, X, if_id;
     /* char *terminal_string; */
{
  SXINT                        bot, top, bot2, top2, x;
  SXINT                        lexeme_id, struct_id, priority, head, fs_id;
  struct if_id2t_list        *ptr;
  struct structure_args_list *ptr2;

  for (bot = inflected_form2display [if_id], top = inflected_form2display [if_id+1];
       bot < top;
       bot++) {
    ptr = if_id2t_list+bot;

    if (ptr->t == X) {
      bot2 = ptr->list;
      top2 = if_id2t_list [bot+1].list;

      while (bot2 < top2) {
	fs_id = call_tfs_instantiation (bot2, Xpq);
	head = fill_heads (Xpq, fs_id);

#if EBUG
	print_f_structure (Xpq, fs_id, SXFALSE);
#endif /* EBUG */

	bot2++;
      }
    }
  }
}

/* On commence par instancier les feuilles terminales */
static void
set_terminal_leaves ()
{
  SXINT  Tpq, T, p, q, trans, tok_no, if_id;
  char *terminal_string;

#if EBUG
  fputs ("Terminal leaves\n", stdout);
#endif /* EBUG */

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    /* T est un terminal */
    T = -Tij2T (Tpq);

#if EBUG
    if (T <= 0)
      sxtrap (ME, "set_terminal_leaves");
#endif /* EBUG */

    p = Tij2i (Tpq);
    q = Tij2j (Tpq);

    if (p >= 0 && q >= 0) {
      /* p et q sont des mlstn */
      p = mlstn2dag_state [p];
      q = mlstn2dag_state [q];
      /* En "standard" on dispose de X et Zforeach sur dag_hd (voir sxearley_main.c) */

      XxYxZ_Xforeach (dag_hd, p, trans) {
	if (XxYxZ_Z (dag_hd, trans) == q) {
	  tok_no = XxYxZ_Y (dag_hd, trans);

	  if (SXBA_bit_is_set (glbl_source [tok_no], T)) {
	    /* La transition tok_no entre p et q contient bien le terminal T */
	    /* La forme flechie correspondante a l'identifiant if_id */
	    if_id = toks_buf [tok_no].lahead;
	    Tij2tok_no [Tpq] = tok_no;
	    /* On peut donc instancier les f-structures lexicales associees a (if_id, T) */
#if EBUG
	    terminal_string = sxstrget (toks_buf [tok_no].string_table_entry);
	    printf ("<%i, %s/%s, %i>(if_id=%i, Tpq=%i)\n",
		    p, terminal_string, spf.inputG.tstring [T], q, if_id, Tpq);
#endif /* EBUG */
	    terminal_instantiation (spf.outputG.maxxnt+Tpq, T, if_id/* , terminal_string */);
	  }
	}
      }
    }
    else {
      /* xt */
      fill_heads (spf.outputG.maxxnt+Tpq, 0);

#if EBUG
      print_f_structure (spf.outputG.maxxnt+Tpq, 0, SXFALSE);
#endif /* EBUG */
    } 
  }
}

/* ... et les non terminaux en erreur */
static void
set_non_terminal_error ()
{
  SXINT Xpq;

  for (Xpq = spf.outputG.maxnt+1; Xpq <= spf.outputG.maxxnt; Xpq++) {
    /* xt */
    fill_heads (Xpq, 0);

#if EBUG
    print_f_structure (Xpq, 0, SXFALSE);
#endif /* EBUG */
  }
}

static SXBOOLEAN
complete_Tij (i, j)
     SXINT i, j;
{
  SXINT     p, q, trans, k, triple, Tij, t;
  SXBOOLEAN ret_val;
  SXBA    glbl_source_i;

  if (i == j) return SXTRUE;

  ret_val = SXFALSE;
  p = mlstn2dag_state [i];
  q = mlstn2dag_state [j];
  /* En "standard" on dispose de X et Zforeach sur dag_hd (voir sxearley_main.c) */

  XxYxZ_Xforeach (dag_hd, p, trans) {
    q = XxYxZ_Z (dag_hd, trans);
    k = dag_state2mlstn [q];

    if (k <= j && complete_Tij (k, j)) {
      ret_val = SXTRUE;
      glbl_source_i = glbl_source [XxYxZ_Y (dag_hd, trans)];
      t = 0;

      while ((t = sxba_scan (glbl_source_i, t)) > 0) {
	MAKE_Tij (Tij, -t, i, j);
      }
    }
  }

  return ret_val;
}


static void
t_completing ()
{
  SXINT Xij, i_rcvr, j_rcvr;

  for (Xij = spf.outputG.maxnt+1; Xij <= spf.outputG.maxxnt; Xij++) {
    i_rcvr = Aij2i (Xij);

    /* On force un chemin terminal entre i_rcvr et j_rcvr */
    /* Les nt de l'error recovery couvrent [i_rcvr..j_rcvr] */
    /* On stocke tous les chemins terminaux entre i_rcvr et j_rcvr */
    if ((j_rcvr = Aij2j (Xij)) > i_rcvr)
      complete_Tij (i_rcvr, j_rcvr);
  }

  maxxt = XxYxZ_top (spf.outputG.Txixj2Tij_hd);
}

static SXINT
tree_sem_pass ()
{
  SXUINT tree_count;
  SXINT               x, leaves_top, nb, unconsistent_nb, f_struct_nb;
  SXBOOLEAN           is_OK;

  is_OK = spf_tree_count (&tree_count);
    
  /* On complete la structure "Tij" avec les terminaux couverts par les nt de la rcvr */
  t_completing ();

#if EBUG
  fprintf (sxtty, "(|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
	   spf.outputG.maxxnt, maxxt, spf.outputG.maxprod, spf.outputG.start_symbol, spf.outputG.maxitem);

  if (is_OK || spf.outputG.start_symbol == 0)
    fprintf (sxtty, "%lu)\n", tree_count);
  else
    fputs ("Cyclic)\n", sxtty);
#endif /* EBUG */

  f_struct_nb = 0;

  if (tree_count != 0) {
    Xpq_top = spf.outputG.maxxnt + maxxt;
    allocate_all ();
    set_terminal_leaves ();
    set_non_terminal_error ();
    leaves_top = DTOP (fs_id_dstack);

    for (tree_id = 0; tree_id < tree_count; tree_id++) {
#if EBUG
      printf ("\nParse tree #%i\n", tree_id+1);
#endif /* EBUG */

      DTOP (fs_id_dstack) = leaves_top;
      is_OK = spf_dag2tree (tree_id, spf.outputG.start_symbol, tree_walk);
      
      if (is_OK && (x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	nb = fs_id_dstack [x];
	unconsistent_nb = sxba_cardinal (unconsistent_heads_set);
	f_struct_nb += nb-unconsistent_nb;

	if (is_consistent = (nb > unconsistent_nb))
	  nb -= unconsistent_nb; /* Les coherentes */
      }
      else {
	nb = 0;
	is_consistent = SXTRUE; /* !! */
      }

#if EBUG
      printf ("After consistency check, there %s %i %sconsistent f_structure%s in the parse tree #%i\n",
	      (nb == 1) ? "is" : "are",
	      nb,
	      is_consistent ? "" : "un",
	      (nb == 1) ? "" : "s",
	      tree_id+1);
#endif /* EBUG */

      if (is_print_f_structure) {
	SXINT head, i;
	SXINT *base_ptr;

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

	    if (!is_consistent || !SXBA_bit_is_set (unconsistent_heads_set, head)) {
	      printf ("\n#%i.%i: ", tree_id+1, i);
	      print_f_structure (XxY_X (heads, head) /* Xpq */,
				 XxY_Y (heads, head) /* fs_id */,
				 !is_consistent);
	    }
	  }
	}

	printf ("*** End of parse tree #%i ***\n", tree_id+1);
      }

      clear_Xpq_set ();
    }

    free_all ();
  }

#if EBUG
  fprintf (sxtty,
	   "There %s %i consistent f_structure%s in this parse forest.\n",
	   (f_struct_nb == 1) ? "is" : "are",
	   f_struct_nb,
	   (f_struct_nb == 1) ? "" : "s");
#endif /* EBUG */

  return 1;
}


/* On est ds le cas ou la foret partagee n'est pas depliee en arbres elementaires mais traitee comme un tout */
static SXBOOLEAN
dag_bu_walk (prod)
     SXINT prod;
{
  tree_walk (prod);
  return SXTRUE; /* On ne touche pas a la foret partagee */
}

#if EBUG
static SXBA valid_fs_id_set, unconsistent_fs_id_set;
#endif /* EBUG */

/* Pour chaque Xpq, On ne conserve que les f_structures de prio maximale */
static void
process_priority ()
{
  SXINT     head_nb, prev_Xpq, x, max_prio, head, Xpq, y, fs_id, prev_y, prio;
  SXINT     *new_head_stack;
  SXBA    new_head_set;

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;
  max_prio = 0;

  for (x = 1; x <= head_nb+1; x++) {
    if (x <= head_nb) {
      head = head_stack [x];
      Xpq = XxY_X (heads, head);
    }

    /* On decoupe le traitement des head par Xpq */
    if (x > head_nb || Xpq != prev_Xpq) {
      /* On change de sous_foret ... */
      if (prev_Xpq) {
	/* ... et ce n'est pas le 1er coup */
	/* On note les heads dont la prio est max_prio */
	for (y = prev_y; y < x; y++) {
	  head = head_stack [y];
	  fs_id = XxY_Y (heads, head);

	  if (get_priority (fs_id) == max_prio) {
	    PUSH (new_head_stack, head);
	    SXBA_1_bit (new_head_set, head);
	  }
	}
      }

      prev_y = x;
      prev_Xpq = Xpq;
      max_prio = 0;
    }

    if (x <= head_nb) {
      fs_id = XxY_Y (heads, head);
      prio = get_priority (fs_id);

      if (prio > max_prio)
	max_prio = prio;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}



/* Calcul du poids des args de fs_id (et des poids de ses sous structures) */
static SXINT
get_args_weight (fs_id)
     SXINT fs_id;
{
  SXINT x, weight, bot, top, val, field_id, static_field_kind, bot2, cur2, top2;

  if (fs_id == 0)
    return 0;

  /*  A FAIRE : Prudence sur les cycliques */
  weight = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;
    val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

    if (field_id == PRED_ID) {
      /* On va prendre tous les arguments, meme si non presents ds pred */
    }
    else {
      if (val > 0) {
	static_field_kind = field_id2kind [field_id];

	if (static_field_kind == STRUCT_KIND) {
	  if (SXBA_bit_is_set (argument_set, field_id))
	    /* C'est un argument */ 
	    weight++;

	  weight += get_args_weight (val);
	}
	else {
	  if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	    /* On suppose que les "set of struct" ne sont pas des arguments !! */
	    for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		 cur2 < top2;
		 cur2++) {
	      val = XH_list_elem (XH_ufs_hd, cur2);
	      weight += get_args_weight (val);
	    }
	  }
	}
      }
    }

    bot++;
  }

  return weight;
}

/* preferer les arguments aux modifieurs */
static void
ranking_by_args ()
{
  /* Pour chaque f_structure Fk on recherche ses sous-structures Fj qui contiennent des pred ayant des 
     arguments instancies on calcule son poids Pj.  Si pi est le poids de l'argi (table statique a fournir, initialement
     on prend 1), on calcule Pj = sigma/i pi puis Pk = sigma/j Pj.  On ne conserve que les f_structure Fk dont le
     poids Pk est maximal.
   */
  SXINT     head_nb, prev_Xpq, x, max_weight, head, Xpq, y, fs_id, prev_y, weight;
  SXINT     *new_head_stack;
  SXBA    new_head_set;

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;
  max_weight = 0;

  for (x = 1; x <= head_nb+1; x++) {
    if (x <= head_nb) {
      head = head_stack [x];
      Xpq = XxY_X (heads, head);
    }

    /* On decoupe le traitement des head par Xpq */
    if (x > head_nb || Xpq != prev_Xpq) {
      /* On change de sous_foret ... */
      if (prev_Xpq) {
	/* ... et ce n'est pas le 1er coup */
	/* On note les heads dont la weight est max_weight */
	for (y = prev_y; y < x; y++) {
	  head = head_stack [y];
	  fs_id = XxY_Y (heads, head);

	  if (get_args_weight (fs_id) == max_weight) {
	    PUSH (new_head_stack, head);
	    SXBA_1_bit (new_head_set, head);
	  }
	}
      }

      prev_y = x;
      prev_Xpq = Xpq;
      max_weight = 0;
    }

    if (x <= head_nb) {
      fs_id = XxY_Y (heads, head);
      weight = get_args_weight (fs_id);

      if (weight > max_weight)
	max_weight = weight;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}




/* Calcul de la distance moyenne des args de fs_id (et de ses sous structures) */
/* retourne ds nb le nb de distances calculees et ds dist le cumul des distances */
static void
get_args_vicinity (fs_id, nb, dist)
     SXINT fs_id, *nb, *dist;
{
  SXINT bot, top, val, field_id, static_field_kind, bot2, cur2, top2;
  SXINT Tpq, p, q, Tij, i, j, store_bot, son_val, d;

  if (fs_id == 0)
    return;

  /*  A FAIRE : Prudence sur les cycliques */

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  val = XH_list_elem (XH_fs_hd, bot);

  field_id = val & FIELD_AND;
  val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

  if (field_id == PRED_ID) {
    /* On va prendre tous les arguments, meme si non presents ds pred */
    Tpq = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)+4);

    if (Tpq) {
      Tpq -= spf.outputG.maxxnt;
      p = Tij2i (Tpq);
      q = Tij2j (Tpq);
      /*
      p = mlstn2dag_state [p];
      q = mlstn2dag_state [q];
      */

      store_bot = bot;
  
      while (++bot < top) {
	val = XH_list_elem (XH_fs_hd, bot);
	field_id = val & FIELD_AND;

	if (SXBA_bit_is_set (argument_set, field_id)) {
	  val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

	  if (val > 0) {
	    son_val =  XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, val));

	    if ((son_val & FIELD_AND) == PRED_ID) {
	      son_val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;
	      Tij = XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, son_val)+4);
	      
	      if (Tij) {
		Tij -= spf.outputG.maxxnt;
		i = Tij2i (Tij);
		j = Tij2j (Tij);
		/*
		i = mlstn2dag_state [i];
		j = mlstn2dag_state [j];
		*/

		if (q < i) {
		  /* Tpq est "avant" Tij */
		  d = i-q;
		}
		else {
		  if (j < p) {
		    /* Tpq est "apres" Tij */
		    d = p-j;
		  }
		  else {
		    /* ?? on dit distance nulle, mais on compte ds la moyenne */
		    d = 0;
		  }
		}

		*nb += 1;
		*dist += d;
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
    val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

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
	    get_args_vicinity (val, nb, dist);
	  }
	}
      }
    }

    bot++;
  }
}


/* Preferer les arguments proches */
static void
ranking_by_vicinity ()
{
  /* Si F = [pred = "a<obj>"; ... obj = [pred = "b<...>", ...], ...] est une sous-structure, on calcule
     la distance entre les lexemes "a" et "b" (regarder quoi noter a l'instanciation des feuilles pour
     pouvoir le calculer ici).  Pour une structure, on calcule la moyenne de ses distances et on prefere
     les structures de distance moyenne minimale.
  */
  SXINT     head_nb, prev_Xpq, x, min_vicinity, head, Xpq, y, fs_id, prev_y, vicinity, nb, dist;
  SXINT     *new_head_stack;
  SXBA    new_head_set;

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;
  min_vicinity = 1000*n;

  for (x = 1; x <= head_nb+1; x++) {
    if (x <= head_nb) {
      head = head_stack [x];
      Xpq = XxY_X (heads, head);
    }

    /* On decoupe le traitement des head par Xpq */
    if (x > head_nb || Xpq != prev_Xpq) {
      /* On change de sous_foret ... */
      if (prev_Xpq) {
	/* ... et ce n'est pas le 1er coup */
	/* On note les heads dont la weight est max_weight */
	for (y = prev_y; y < x; y++) {
	  head = head_stack [y];
	  fs_id = XxY_Y (heads, head);
	  nb = dist = 0;
	  get_args_vicinity (fs_id, &nb, &dist);

	  vicinity = nb ? (1000*dist)/nb : 1000*n;

	  if (vicinity == min_vicinity) {
	    PUSH (new_head_stack, head);
	    SXBA_1_bit (new_head_set, head);
	  }
	}
      }

      prev_y = x;
      prev_Xpq = Xpq;
      min_vicinity = 1000*n;
    }

    if (x <= head_nb) {
      fs_id = XxY_Y (heads, head);
      nb = dist = 0;
      get_args_vicinity (fs_id, &nb, &dist);

      vicinity = nb ? (1000*dist)/nb : 1000*n;

      if (vicinity < min_vicinity)
	min_vicinity = vicinity;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}

/* Calcul du niveau d'enchassement de fs_id */
static SXINT
get_args_embedding (fs_id)
     SXINT fs_id;
{
  SXINT x, embedding, max_embedding, bot, top, val, field_id, static_field_kind, bot2, cur2, top2;

  if (fs_id == 0)
    return 0;

  /*  A FAIRE : Prudence sur les cycliques */
  max_embedding = 0;

  bot = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (bot < top) {
    val = XH_list_elem (XH_fs_hd, bot);

    field_id = val & FIELD_AND;
    val >>= MAX_FIELD_ID_SHIFT+STRUCTURE_NAME_shift;

    if (field_id == PRED_ID) {
      /* On va prendre tous les arguments, meme si non presents ds pred */
    }
    else {
      if (val > 0) {
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
	      embedding = 1+get_args_embedding (val);
	      
	      if (embedding > max_embedding)
		max_embedding = embedding;
	    }
	  }
	}
      }
    }

    bot++;
  }

  return max_embedding;
}

/* Preferer les structures les + enchassees */
static void
ranking_by_embedding ()
{
  /* On definit une mesure de l'enchassement :
     1ere tentative : profondeur de la structure la + enchassee
   */
  SXINT     head_nb, prev_Xpq, x, max_embedding, head, Xpq, y, fs_id, prev_y, embedding;
  SXINT     *new_head_stack;
  SXBA    new_head_set;

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;
  max_embedding = 0;

  for (x = 1; x <= head_nb+1; x++) {
    if (x <= head_nb) {
      head = head_stack [x];
      Xpq = XxY_X (heads, head);
    }

    /* On decoupe le traitement des head par Xpq */
    if (x > head_nb || Xpq != prev_Xpq) {
      /* On change de sous_foret ... */
      if (prev_Xpq) {
	/* ... et ce n'est pas le 1er coup */
	/* On note les heads dont la embedding est max_embedding */
	for (y = prev_y; y < x; y++) {
	  head = head_stack [y];
	  fs_id = XxY_Y (heads, head);

	  if (get_args_embedding (fs_id) == max_embedding) {
	    PUSH (new_head_stack, head);
	    SXBA_1_bit (new_head_set, head);
	  }
	}
      }

      prev_y = x;
      prev_Xpq = Xpq;
      max_embedding = 0;
    }

    if (x <= head_nb) {
      fs_id = XxY_Y (heads, head);
      embedding = get_args_embedding (fs_id);

      if (embedding > max_embedding)
	max_embedding = embedding;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}

static SXINT special_atomic_field_id;

static SXINT
get_special_atomic_field (fs_id)
     SXINT fs_id;
{
  SXINT bot, cur, top, val, field_id, local_atom_id, *local2atom_id;

  bot = cur = XH_X (XH_fs_hd, fs_id);
  top = XH_X (XH_fs_hd, fs_id+1);
	
  if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
    /* shared */
    top--;

  while (cur < top) {
    val = XH_list_elem (XH_fs_hd, cur);
    field_id = val & FIELD_AND;

    if (field_id == special_atomic_field_id) {
      val >>= FS_ID_SHIFT;
      local2atom_id = field_id2atom_field_id [field_id];
      local_atom_id = 0;

      while ((val >>= 1) != 0) {
	local_atom_id++;

	if (val & 1)
	  return local_atom_id;
      }
    }

    cur++;
  }

  return 0;
}


static void
ranking_by_special_atomic_field ()
{
  SXINT     head_nb, prev_Xpq, x, max_prio, head, Xpq, y, fs_id, prev_y, prio, id;
  SXINT     *new_head_stack;
  SXBA    new_head_set;
  char    *str;

  /* On cherche l'id de "mode" */
  /* devra etre fait statiquement de facon generique */
  for (id = 2; id <= MAX_FIELD_ID; id++) {
    str = field_id2string [id];

    if (strlen (str) == 4 && strncmp (str, "mode", 4)) {
      special_atomic_field_id = id;
      break;
    }
  }

  if (special_atomic_field_id == 0)
    return; /* garde-fou */

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;
  max_prio = sizeof (atom_id2string) / sizeof (char*);

  for (x = 1; x <= head_nb+1; x++) {
    if (x <= head_nb) {
      head = head_stack [x];
      Xpq = XxY_X (heads, head);
    }

    /* On decoupe le traitement des head par Xpq */
    if (x > head_nb || Xpq != prev_Xpq) {
      /* On change de sous_foret ... */
      if (prev_Xpq) {
	/* ... et ce n'est pas le 1er coup */
	/* On note les heads dont la prio est max_prio */
	for (y = prev_y; y < x; y++) {
	  head = head_stack [y];
	  fs_id = XxY_Y (heads, head);

	  if (get_special_atomic_field (fs_id) == max_prio) {
	    PUSH (new_head_stack, head);
	    SXBA_1_bit (new_head_set, head);
	  }
	}
      }

      prev_y = x;
      prev_Xpq = Xpq;
      max_prio = 0;
    }

    if (x <= head_nb) {
      fs_id = XxY_Y (heads, head);
      prio = get_special_atomic_field (fs_id);

      if (prio > 0 && prio < max_prio)
	max_prio = prio;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}


/* La ou les f_structures vien[nen]t d'etre calculee[s] sur la foret partagee */
/* On la parcourt top-down pour eliminer les noeuds et les f_structures inutiles */
static SXBOOLEAN
dag_td_walk (Aij_or_prod)
     SXINT Aij_or_prod;
{
  SXINT        prod, i, x, rule, fs_id, head, Xpq, pos, d;
  SXINT        *local_head_stack, *base_ptr;
  SXBOOLEAN    is_valid, is_root;
  struct lhs *plhs;
  struct rhs *p;

  if (Aij_or_prod > 0)
    /* Aij */
    return SXTRUE;

  /* prod */
  prod = -Aij_or_prod;

  if ((x = Pij2disp [prod]) == 0) {
    /* Echec */
    return SXFALSE;
  }

  is_valid = SXFALSE;
  local_head_stack = fs_id_Pij_dstack+x;
  plhs = spf.outputG.lhs+prod;
  Aij = plhs->lhs /* static */;
  is_root = (Aij == spf.outputG.start_symbol);
  prhs = spf.outputG.rhs+plhs->prolon-1; /* static */

  for (rule = prod2rule [plhs->init_prod]; rule != 0; rule = rule2next_rule [rule]) {
    if (x = *local_head_stack) {
      for (i = 1; i <= x; i++) {
	head = local_head_stack [i++];	      

#if EBUG
	if (XxY_X (heads, head) != Aij)
	  sxtrap (ME, "dag_td_walk");
#endif /* EBUG */

	if (!is_root /* On est sur un noeud interieur ... */ && SXBA_bit_is_set (valid_heads_set, head) /* ... valide' */
	    || SXBA_bit_is_set (head_set, head) /* ... ou a la racine sur une tete selectionnee */) {
	  doldol = local_head_stack [i];
	  fs_id = XxY_Y (heads, head);

	  is_valid = SXTRUE;

#if EBUG
	  SXBA_1_bit (valid_fs_id_set, fs_id);

	  if (SXBA_bit_is_set (unconsistent_heads_set, head))
	    SXBA_1_bit (unconsistent_fs_id_set, fs_id);
#endif /* EBUG */

	  /* On valide les head des fils */
	  if (doldol >= 0) {
	    /* doldol == -1 => bloc d'equations vide */
	    p = prhs;

	    while ((Xpq = (++p)->lispro) != 0) {
	      base_ptr = fs_id_dstack + Xpq2disp [(Xpq < 0) ? spf.outputG.maxxnt-Xpq : Xpq];

	      d = *base_ptr++;
	      head = base_ptr [doldol % d];

	      if (Xpq > 0)
		SXBA_1_bit (valid_heads_set, head);
				 
	      doldol /= d;
	    }
	  }
	}
      }
    }
	
    local_head_stack += x+1;
  }
   
#if EBUG
  /* valid_fs_id_set contient l'ensemble des fs_id valides pour (toutes les rules de) prod */
  if (is_valid) {
    fputs ("\n", stdout);
    spf_print_prod (stdout, prod, SXTRUE /* with nl */);

    x = sxba_cardinal (valid_fs_id_set);
    i = 0;
    fs_id = -1;

    while ((fs_id = sxba_scan_reset (valid_fs_id_set, fs_id)) >= 0) {   
      i++;
      printf ("#%i/%i: ", i, x);
      print_f_structure (Aij, fs_id, SXBA_bit_is_set (unconsistent_fs_id_set, fs_id));
      SXBA_0_bit (unconsistent_fs_id_set, fs_id);
    }
  }
#endif /* EBUG */

  return is_valid;
}

static void
xml_gen_header ()
{
    fprintf (xml_file,
	     "\n\
<!-- ********************************************************************\n\
\tThe file \"%s\" contains the LFG parse forest in XML format\n\
\twhich has been generated\
\tby the SYNTAX(*) LFG processor SXLFG(*)\n\
     ********************************************************************\n\
\t(*) SYNTAX and SXLFG are trademarks of INRIA.\n\
     ******************************************************************** -->\n\n",
	     xml_file_name);
}

static SXINT
xml_print_terminal_leaves ()
{
  SXINT  Tpq, T, p, q, trans, tok_no, t_nb;
  char *terminal_string;

  fputs ("<!-- ************************* TERMINAL SYMBOLS ************************* -->\n", xml_file); 

  t_nb = 0;

  for (Tpq = 1; Tpq <= maxxt; Tpq++) {
    /* T est un terminal */
    T = -Tij2T (Tpq);

    p = Tij2i (Tpq);
    q = Tij2j (Tpq);

    if (p >= 0 && q >= 0) {
      /* p et q sont des mlstn */
      p = mlstn2dag_state [p];
      q = mlstn2dag_state [q];
      /* En "standard" on dispose de X et Zforeach sur dag_hd (voir sxearley_main.c) */

      XxYxZ_Xforeach (dag_hd, p, trans) {
	if (XxYxZ_Z (dag_hd, trans) == q) {
	  tok_no = XxYxZ_Y (dag_hd, trans);

	  if (SXBA_bit_is_set (glbl_source [tok_no], T)) {
	    /* La transition tok_no entre p et q contient bien le terminal T */
	    terminal_string = sxstrget (toks_buf [tok_no].string_table_entry);
	    t_nb++;
	    fprintf (xml_file,
		     " <leaf id=\"T%i\" kind=\"t\" name=\"%s\" form=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
		     Tpq,
		     spf.inputG.tstring [T],
		     terminal_string,
		     p,
		     q
		     );
	  }
	}
      }
    }
    else {
      /* Cas d'un xt */
      t_nb++;
      fprintf (xml_file,
	       " <leaf id=\"T%i\" kind=\"xt\" name=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
	       Tpq,
	       spf.inputG.tstring [T],
	       p,
	       q
	       );
      
    }
  } 

  return t_nb;
}


static void
xml_print_nt_set (Aij_set)
     SXBA Aij_set;
{
  SXINT Aij;
  
  fputs ("<!-- ************************* NONTERMINAL SYMBOLS ************************* -->\n", xml_file); 

  Aij = 0;

  while ((Aij = sxba_scan (Aij_set, Aij)) > 0) {
    if (Aij <= spf.outputG.maxnt)
      /* nt */
      fprintf (xml_file,
	       " <node id=\"N%i\" kind=\"nt\" name=\"%s\" lb=\"%i\" ub=\"%i\" hook_id=\"H%i\"/>\n",
	       Aij,
	       spf.inputG.ntstring [Aij2A (Aij)],
	       Aij2i (Aij),
	       Aij2j (Aij),
	       Aij
	       );
    else {
      /* xnt */
      fprintf (xml_file,
	       " <node id=\"N%i\" kind=\"xnt\" name=\"%s\" lb=\"%i\" ub=\"%i\"/>\n",
	       Aij,
	       spf.inputG.ntstring [Aij2A (Aij)],
	       Aij2i (Aij) /* -1 => undef */,
	       Aij2j (Aij) /* -1 => undef */
	       );
      /* On enleve les xnt pour la suite */
      SXBA_0_bit (Aij_set, Aij);
    }
  }
}

static void
xml_print_forest ()
{
  SXINT  Aij, hook, prod, item, Xpq, t_nb;
  SXBA prod_set, Aij_set;

  prod_set = sxba_calloc (spf.outputG.maxprod+1);
  Aij_set = sxba_calloc (spf.outputG.maxxnt+1);

  for (Aij = 1; Aij <= spf.outputG.maxnt; Aij++) {
    /* nt ... */
    if (Aij2i (Aij) < Aij2j (Aij)) {
      /* ... non vide */
      /* On ne sort ni les nt vides ni leurs nt_prod */
      hook = spf.outputG.lhs [spf.outputG.maxprod+Aij].prolon;

      while ((prod = spf.outputG.rhs [hook].lispro) != 0) {
	/* On parcourt les productions dont la lhs est Aij */
	if (prod > 0) {
	  SXBA_1_bit (prod_set, prod);
	  SXBA_1_bit (Aij_set, Aij);

	  /* On capture aussi les xnt */
	  item = spf.outputG.lhs [prod].prolon;

	  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	    if (Xpq > 0)
	      /* nt ou xnt */
	      SXBA_1_bit (Aij_set, Xpq);
	  }
	}

	hook++;
      }
    }
  }

  t_nb = xml_print_terminal_leaves ();

  /* Sortir les non-terminaux */
  xml_print_nt_set (Aij_set);
  /* Les xnt ont ete enleves de Aij_set */
  
  fputs ("<!-- ************************* PARSE FOREST ************************* -->\n", xml_file); 

  fprintf (xml_file,
	   " <parse_forest id=\"sentence%i\" axiome=\"N%i\" t_nb=\"%i\" nt_nb=\"%i\" prod_nb=\"%i\">\n",
	   sentence_id,
	   spf.outputG.start_symbol,
	   t_nb,
	   sxba_cardinal (Aij_set),
	   sxba_cardinal (prod_set)
	   );

  prod = 0;

  while ((prod = sxba_scan (prod_set, prod)) > 0) {
    item = spf.outputG.lhs [prod].prolon;
    Aij = spf.outputG.lhs [prod].lhs;

    fputs ("  <!-- ", xml_file);
    spf_print_prod (xml_file, prod, SXFALSE /* no nl */);
    fputs (" -->\n", xml_file);

    fprintf (xml_file,
	     "  <rule id=\"R_%i_%i\">\n",
	     prod,
	     spf.outputG.lhs [prod].init_prod);

    fprintf (xml_file, "   <lhs node_id=\"N%i\"/>\n", Aij);

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      /* On parcourt la RHS de prod */
      if (Xpq > 0) {
	/* nt ... */
	if (Xpq > spf.outputG.maxnt || Aij2i (Xpq) < Aij2j (Xpq)) {
	  /* ... non vide ou xnt */
	  fprintf (xml_file, "    <rhs node_id=\"N%i\"/>\n", Xpq);
	}
      }
      else {
	/* T */
	fprintf (xml_file, "    <rhs leaf_id=\"T%i\"/>\n", -Xpq);
      }
    }

    fputs ("  </rule>\n", xml_file);
  }

  Aij = 0;

  while ((Aij = sxba_scan (Aij_set, Aij)) > 0) {
    hook = spf.outputG.lhs [spf.outputG.maxprod+Aij].prolon;

    fprintf (xml_file, "  <hook id=\"H%i\">\n", Aij);

    while ((prod = spf.outputG.rhs [hook].lispro) != 0) {
      /* On parcourt les productions dont la lhs est Aij */
      if (prod > 0) {
	/* Non vide */
	fprintf (xml_file, "   <rule_ref rule_id=\"R_%i_%i\"/>\n", prod, spf.outputG.lhs [prod].init_prod);
      }

      hook++;
    }

    fputs ("  </hook>\n", xml_file);
  }

  fputs (" </parse_forest>\n", xml_file);

  sxfree (prod_set);
  sxfree (Aij_set);
}


static void
xml_print_f_structure ()
{
  SXINT             fs_id, bot, top, bot2, cur2, top2, bot3, cur3, top3, max_priority, priority;
  SXINT             val, field_id, field_kind, sous_cat_id, atom_id, local_atom_id, x, i, Tpq, id;
  SXINT             *local2atom_id;
  SXBOOLEAN         is_optional, is_first2;
  struct pred_val *ptr2;
  unsigned char   static_field_kind;

  /* traite' comme une queue */
  x = 0; /* indice ds fs_id_stack */

  while (++x <= fs_id_stack [0]) {
    fs_id = fs_id_stack [x];

    bot = XH_X (XH_fs_hd, fs_id);
    top = XH_X (XH_fs_hd, fs_id+1);

    if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0) {
      top--;
      fprintf (xml_file, " <fs id=\"FS%i\" shared=\"true\">\n", fs_id);
    }
    else
      fprintf (xml_file, " <fs id=\"FS%i\">\n", fs_id);

    while (bot < top) {
      val = XH_list_elem (XH_fs_hd, bot);

      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];
      val >>= MAX_FIELD_ID_SHIFT;
      field_kind = val & KIND_AND;
      val >>= STRUCTURE_NAME_shift;

      if (field_id == PRED_ID) {
	SXINT *dpv_ptr = &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

	if ((priority = dpv_ptr [3]) > max_priority)
	  max_priority = priority;
      
	fputs ("  <pred>\n", xml_file);

	fprintf (xml_file, "   <lexeme name=\"%s\"/>\n", lexeme_id2string [dpv_ptr [0] /* lexeme_id */]);

	sous_cat_id = dpv_ptr [1] /* sous_cat1 */;

	for (i = 1; i <= 2; i++) {
	  if (sous_cat_id) {
	    fprintf (xml_file, "   <scat%i>\n", i);
      
	    for (bot2 = cur2 = sous_cat [sous_cat_id], top2 = sous_cat [sous_cat_id+1];
		 cur2 < top2;
		 cur2++) {
	      fputs ("    <fct name=\"", xml_file);

	      field_id = sous_cat_list [cur2];
	
	      if (field_id < 0) {
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
		  fprintf (xml_file, "%s%s", (cur3 != bot3) ? "|" : "", field_id2string [field_id]);
		}
	      }
	      else {
		fprintf (xml_file, "%s", field_id2string [field_id]);
	      }

	      if (is_optional)
		fputs ("\" optional=\"yes\"/>\n", xml_file);
	      else
		fputs ("\"/>\n", xml_file);
	    }

	    fprintf (xml_file, "   </scat%i>\n", i);
	  }

	  if (i==1)
	    sous_cat_id = dpv_ptr [2] /* sous_cat2 */;
	}

	if (Tpq = dpv_ptr [4]) {
	  Tpq -=  spf.outputG.maxxnt;
	  fprintf (xml_file, "   <leaf_ref leaf_id=\"T%i\"/>\n", Tpq);
	} 
      
	fputs ("  </pred>\n", xml_file);
      }
      else {
	fprintf (xml_file, "  <field name=\"%s\"", field_id2string [field_id]);

	if (static_field_kind & STRUCT_KIND) {
	  if (static_field_kind & UNBOUNDED_KIND)
	    fputs (" type=\"set\"", xml_file);
	  else
	    fputs (" type=\"struct\"", xml_file);
	}
	else {
	  /* atomique ... */
	  if (static_field_kind & UNBOUNDED_KIND) {
#if ATOM_Aij || ATOM_Pij
	    if (static_field_kind & Aij_KIND)
	      fputs (" type=\"c_struct\"", xml_file);
	    else
#endif /* ATOM_Aij || ATOM_Pij */
	      fputs (" type=\"atom\"", xml_file);
	  }
	  else
	    fputs (" type=\"atom\"", xml_file);
	}
	    
	fputs (" val=\"", xml_file);

	if (static_field_kind & ATOM_KIND) {
	  if (static_field_kind & UNBOUNDED_KIND) {
#if ATOM_Aij || ATOM_Pij
	    if (static_field_kind & Aij_KIND) {
	      if (val) {
		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  if (cur2 > bot2)
		    fputs ("|", xml_file);

		  id = XH_list_elem (XH_ufs_hd, cur2);

		  if (id > spf.outputG.maxxnt) { 
		    id -= spf.outputG.maxxnt; /* Pij */
		    fprintf (xml_file, "R_%i_%i", id, spf.outputG.lhs [id].init_prod);
		  }
		  else
		    fprintf (xml_file, "N%i", id /* Aij */);
		}
	      }
	      else
		fputs ("ERRORij", xml_file);
	    }
	    else
#endif /* ATOM_Aij || ATOM_Pij */
	      fprintf (xml_file, "%s", atom_id2string [val]);
	  }
	  else {
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
		  fputs ("|", xml_file);

		fprintf (xml_file, "%s", atom_id2string [atom_id]);
	      }
	    }
	  }
	}
	else {
	  if (static_field_kind == STRUCT_KIND) {
	    if (val == 0) {
	      fputs ("0", xml_file);
	    }
	    else {
	      if (SXBA_bit_is_reset_set (fs_id_set, val))
		PUSH (fs_id_stack, val);
		
	      fprintf (xml_file, "FS%i", val);
	    }
	  }
	  else {
	    if (static_field_kind == STRUCT_KIND+UNBOUNDED_KIND) {
	      if (val == 0)
		fputs ("0", xml_file);
	      else {
		for (cur2 = bot2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
		     cur2 < top2;
		     cur2++) {
		  if (cur2 > bot2)
		    fputs ("|", xml_file);
		
		  val = XH_list_elem (XH_ufs_hd, cur2);
		
		  if (SXBA_bit_is_reset_set (fs_id_set, val))
		    PUSH (fs_id_stack, val);

		  fprintf (xml_file, "FS%i", val);
		}
	      }
	    }
	  }
	}

	fputs ("\"/>\n", xml_file);
      }

      bot++;
    }

    fputs (" </fs>\n", xml_file);
  }

  sxba_empty (fs_id_set);
}

/* Remplit head_stack avec toutes les head associees a Xpq qui verifient la condition (*f)(head) */
static SXINT
fill_head_stack (Xpq, f)
     SXINT Xpq;
     SXBOOLEAN (*f)();
{
  SXINT x, d, head, fs_id;
  SXINT *base_ptr;
  
  x = Xpq2disp [Xpq];

  if (x) {
    base_ptr = fs_id_dstack + x;
    d = *base_ptr++;

    while (d--) {
      head = *base_ptr++;
      fs_id = XxY_Y (heads, head);
	
      if (fs_id > 0 && f (head)) {
	if (SXBA_bit_is_reset_set (head_set, head))
	  PUSH (head_stack, head);
      }
    }
  }

  return x;
}

#if 0
/* Remplace' le 14/06/04 par fill_head_stack () */
static SXINT
fill_fs_id_stack (Xpq)
     SXINT Xpq;
{
  SXINT x, d, head, fs_id;
  SXINT *base_ptr;
  
  x = Xpq2disp [Xpq];

  if (x) {
    base_ptr = fs_id_dstack + x;
    d = *base_ptr++;

    while (d--) {
      head = *base_ptr++;
      fs_id = XxY_Y (heads, head);
	
      if (fs_id > 0 && SXBA_bit_is_reset_set (fs_id_set, fs_id))
	PUSH (fs_id_stack, fs_id);
    }
  }

  return x;
}
#endif /* 0 */

static SXBOOLEAN
filter_root (head)
     SXINT head;
{
  return (!is_consistent || !SXBA_bit_is_set (unconsistent_heads_set, head));
}

static SXBOOLEAN
filter_true (head)
     SXINT head;
{
  return SXTRUE;
}


/* Aucune f_structure a la racine ... */
/* ... on essaie de sauver les meubles */
/* On va noter ds head_stack toutes les head de + haut niveau que l'on va trouver, pas de contrainte sur la coherence  */
static SXBOOLEAN
rcvr_bu_walk (prod)
     SXINT prod;
{
  SXINT     item, Xpq;
  SXBOOLEAN has_f_structures;

  has_f_structures = (Xpq2disp [spf.outputG.lhs [prod].lhs] > 0);
  item = spf.outputG.lhs [prod].prolon;

  while ((Xpq = spf.outputG.rhs [item].lispro) != 0) {
    if (Xpq < 0 /* t ... */)
      /* On capture les terminaux de la RHS */
      fill_head_stack (spf.outputG.maxxnt-Xpq, filter_true);
    else {
      /* nt ou xnt */
      if (!has_f_structures && Xpq2disp [Xpq] > 0)
	/* Le pere n'a pas de f_structures, ce fils est donc maximal */
	fill_head_stack (Xpq, filter_true);
    }

    item++;
  }

  return SXTRUE; /* On ne touche pas a la foret partagee */
}
#if 0
/* Le 22/07/04 on traite par une passe bu */
static SXBOOLEAN
rcvr_td_walk (Aij_or_prod)
     SXINT Aij_or_prod;
{
  SXINT  item, Xpq;

  if (Aij_or_prod < 0) {
    /* prod */
    /* On capture les terminaux de la RHS */
    item = spf.outputG.lhs [-Aij_or_prod].prolon;
      
    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      /* On parcourt la RHS de la prod instanciee */
      if (Xpq < 0 /* t ... */) {
	fill_head_stack (spf.outputG.maxxnt-Xpq, filter_true);
#if 0
	fill_fs_id_stack (spf.outputG.maxxnt-Xpq);
#endif /* 0 */
      }
    }

    return SXTRUE;
  }

  /* Aij */
  return fill_head_stack (Aij_or_prod, filter_true) == 0; /* On continue la descente */
#if 0
  return fill_fs_id_stack (Aij_or_prod) == 0; /* On continue la descente */
#endif /* 0 */
}
#endif /* 0 */


static void
xml_print_f_structures ()
{
  SXINT        head, x, y, fs_id, cur_nb, Xpq, hook, prod, rule, i;
  SXINT        *local_head_stack;
  struct lhs *plhs;
#if EBUG
  SXBOOLEAN    found;
#endif /* EBUG */

  fputs ("<!-- ************************* F_STRUCTURES ************************* -->\n", xml_file); 

  cur_nb = 0;

  for (x = 1; x <= head_stack [0]; x++) {
    head = head_stack [x];
    fs_id = XxY_Y (heads, head);

    if (SXBA_bit_is_reset_set (fs_id_set, fs_id)) {
      PUSH (fs_id_stack, fs_id);
      cur_nb++;
    }
  }

  fprintf (xml_file, " <f_structures kind=\"%s\" relaxed=\"%s\" consistent=\"%s\" ranking_kind=\"%i\" nb=\"%i\">\n",
	   is_rcvr_done ? "partial" : "complete",
	   is_relaxed_run ? "yes" : "no",
	   is_relaxed_run ? "no" : (is_consistent ? "yes" : "no"),
	   ranking_kind,
	   cur_nb);

  /* Le 16/06/04 On associe la production (ou le terminal) qui a produit la f_structure */
#if EBUG
  found = SXFALSE;
#endif /* EBUG */

  for (y = 1; y <= head_stack [0]; y++) {
    head = head_stack [y];
    Xpq = XxY_X (heads, head);
    fs_id = XxY_Y (heads, head);

    if (Xpq > spf.outputG.maxxnt) {
      /* C'est une feuille terminale */
      Xpq -= spf.outputG.maxxnt;
      fprintf (xml_file, "  <fs_ref fs_id=\"FS%i\" leaf_id=\"T%i\"/>\n", fs_id, Xpq);
    }
    else {
      /* C'est un non terminal */
      /* On cherche toutes les Xpq-prods */
      hook = spf.outputG.lhs [spf.outputG.maxprod+Xpq].prolon;

      while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
	/* head peut etre trouve pour des prod differentes */
	if (prod < 0)
	  /* On doit considerer les prod "effacees" car on a efface' les prods de tous les noeuds
	     ayant des f_structures de + haut niveau */
	  prod = -prod;

	if (x = Pij2disp [prod]) {
	  /* ... valide */
	  local_head_stack = fs_id_Pij_dstack+x;
	  plhs = spf.outputG.lhs+prod;

	  for (rule = prod2rule [plhs->init_prod]; rule != 0; rule = rule2next_rule [rule]) {
	    if (x = *local_head_stack) {
	      for (i = 1; i <= x; i++) {
		if (head == local_head_stack [i++]) {
		  /* C'est une bonne */
		  fprintf (xml_file, "  <fs_ref fs_id=\"FS%i\" rule_id=\"R_%i_%i\"/>\n", fs_id, prod, plhs->init_prod);
#if EBUG
		  found = SXTRUE;;
#endif /* EBUG */
		  break;
		}	      
	      }

	      if (i <= x)
		/* une seule rule par prod */
		break;
	    }
	
	    local_head_stack += x+1;
	  }
	}
      }

#if EBUG
      if (!found)
	/* head doit etre trouve' */
	sxtrap (ME, "xml_print_f_structures");
      else
	found = SXFALSE;
#endif /* EBUG */
    }
  }

  fputs (" </f_structures>\n", xml_file);

  xml_print_f_structure ();
}



static void
xml_output ()
{
  SXINT tok_no;

  fputs ("<?xml version=\"1.0\" encoding=\"ISO-8859-1\" standalone=\"yes\"?>\n", xml_file); 

  xml_gen_header ();

  fputs ("<sxlfgDocument>\n", xml_file); 
  fputs (" <sentence input=\"", xml_file);

  for (tok_no = 1; tok_no <= last_tok_no; tok_no++) {
    fprintf (xml_file, "%s%s", sxstrget (toks_buf [tok_no].string_table_entry), tok_no == last_tok_no ? "" : " ");
  }

  fputs ("\"/>\n", xml_file);

  xml_print_forest ();

  xml_print_f_structures ();
  
  fputs ("</sxlfgDocument>\n", xml_file); 
}

#ifdef EASY
/* On est ds la campagne d'evaluation EASY */
#include "sxword.h"

static XxYxZ_header  easy_hd, easy_Tij_hd;
static SXBA          easy_constituent_nt_set;
#if 0
static SXBA          easy_constituent_t_set;
#endif /* 0 */
static SXINT           *easy_constituent_stack, *easy_Tij_stack;
static VARSTR        easy_vstr;
static sxword_header easy_hrefs;
static SXINT           *Xpq2easy_href;

static SXINT           subj_id, cat_id, obj_id, aij_id, vcomp_id, scomp_id, acomp_id, pcas_id;
static SXINT           obj_ids [12]; /* initialises avec les valeurs suivantes */
static SXINT           a_obj_id, de_obj_id, par_obj_id, devant_obj_id, dans_obj_id, sur_obj_id, en_obj_id, contre_obj_id, apres_obj_id,
                     avec_obj_id, vers_obj_id, pour_obj_id;
static SXINT           vcomp_ids [3]; /* initialises avec les valeurs suivantes */
static SXINT           a_vcomp_id, de_vcomp_id, par_vcomp_id;
static SXINT           scomp_ids [3]; /* initialises avec les valeurs suivantes */
static SXINT           scomp_id, a_scomp_id, de_scomp_id;
static SXINT           acomp_ids [3]; /* initialises avec les valeurs suivantes */
static SXINT           acomp_id, de_acomp_id, pour_acomp_id;
static SXINT           adjunct_id;

static SXINT           atom_v, atom_adv, atom_nc, atom_adj, atom_prep;
static SXINT           Ei_lgth;

/* Met ds fs_id_stack tous les fs_id des sous-structures accessibles depuis le parametre */
static void
fill_fs_id_stack (fs_id)
     SXINT fs_id;
{
  SXINT bot, top, val, field_id, cur2, top2, x;
  unsigned char   static_field_kind;

  fs_id_stack [0] = 0;
  sxba_empty (fs_id_set);
  SXBA_1_bit (fs_id_set, fs_id);
  PUSH (fs_id_stack, fs_id);

  x = 1;

  do {
    bot = XH_X (XH_fs_hd, fs_id);
    top = XH_X (XH_fs_hd, fs_id+1);
      
    if (bot < top && XH_list_elem (XH_fs_hd, top-1) < 0)
      top--;

    while (bot < top) {
      val = XH_list_elem (XH_fs_hd, bot);

      field_id = val & FIELD_AND;
      static_field_kind = field_id2kind [field_id];
      val >>= FS_ID_SHIFT;

      if ((static_field_kind & STRUCT_KIND) && val) {
	if (static_field_kind == STRUCT_KIND) {
	  if (SXBA_bit_is_reset_set (fs_id_set, val))
	    PUSH (fs_id_stack, val);
	}
	else {
	  for (cur2 = XH_X (XH_ufs_hd, val), top2 = XH_X (XH_ufs_hd, val+1);
	       cur2 < top2;
	       cur2++) {
		
	    val = XH_list_elem (XH_ufs_hd, cur2);
		
	    if (SXBA_bit_is_reset_set (fs_id_set, val))
	      PUSH (fs_id_stack, val);
	  }
	}
      }

      bot++;
    }

    if (++x > fs_id_stack [0])
      /* Pas de nl pour Priority ... */
      fs_id = 0;
    else
      fs_id = fs_id_stack [x];
  } while (fs_id);
}

/* Pour chaque Xpq, On ne conserve qu'une seule f_structure (la 1ere) */
/* Si (q-p < 2) les f_strucures sont supprimees (calcul de relations -au moins- binaires entre des composants non vides) */
static void
single_f_structure_filtering ()
{
  SXINT     head_nb, prev_Xpq, x, head, Xpq;
  SXINT     *new_head_stack;
  SXBA    new_head_set;

  new_head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), new_head_stack [0] = 0;
  new_head_set = sxba_calloc (XxY_top (heads)+1);

  head_nb = head_stack [0];
  prev_Xpq = 0;

  for (x = 1; x <= head_nb; x++) {
    head = head_stack [x];
    Xpq = XxY_X (heads, head);

    if (Xpq != prev_Xpq) {
      if (Xpq <= spf.outputG.maxnt && (Aij2j (Xpq) - Aij2i (Xpq)) >= 2) {
	/* ni les terminaux, ni les nt de la rcvr ne sont interessants */
	PUSH (new_head_stack, head);
	SXBA_1_bit (new_head_set, head);
      }

      prev_Xpq = Xpq;
    }
  }

  sxfree (head_stack);
  sxfree (head_set);
  head_stack = new_head_stack;
  head_set = new_head_set;
}


static SXBOOLEAN
tpath2easy (i, j)
     SXINT i, j;
{
  SXINT     trans, k, triple, Tij;
  SXBOOLEAN ret_val;

  if (i == j) return SXTRUE;

  ret_val = SXFALSE;

  Tij_iforeach (i, Tij) {
    k = Tij2j (Tij); /* k > i */

    if (k <= j && tpath2easy (k, j)) {
      ret_val = SXTRUE;
      XxYxZ_set (&easy_Tij_hd, i, Tij, k, &triple);
    }
  }

  return ret_val;
}

/* Sert ds easy_td_walk pour reperer si on est ds un constituant max */
static SXBA max_constituent_set;

static SXBOOLEAN
is_a_maximal_constituent (i, j)
     SXINT i, j;
{
  SXINT k;

  if (j <= i) return SXFALSE;

  k = i;

  while (k < j) {
    if (SXBA_bit_is_set (max_constituent_set, k))
      return SXFALSE;

    k++;
  }

  while (i < j) {
    SXBA_1_bit (max_constituent_set, i);
    i++;
  }

  return SXTRUE;
}

static SXBOOLEAN
easy_td_walk (Aij)
     SXINT Aij;
{
  SXINT Xpq, A, triple, trans, prod, item, Tpq, t, i, j, i_rcvr, j_rcvr;

  if (Aij < 0) {
    /* prod */
    prod = -Aij;
    /* On capture les terminaux de la RHS */
    item = spf.outputG.lhs [prod].prolon;
      
    i_rcvr = 0;

    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      /* On parcourt la RHS de la prod instanciee */
      /* On est obligatoirement hors constituant easy, les terminaux trouves permettent donc
	 de "faire la liaison" entre les constituants easy */
      if (Xpq > 0) {
	if (Xpq > spf.outputG.maxnt) {
	  /* nt de la rcvr */
	  if (i_rcvr == 0) {
	    i_rcvr = Aij2i (Xpq);
	  }

	  if ((j_rcvr = Aij2j (Xpq)) > 0) {
	    /* On force un chemin terminal entre i_rcvr et j_rcvr */
	    /* Les nt de l'error recovery couvrent [i_rcvr..j_rcvr] */
	    /* On stocke tous les chemins terminaux entre i_rcvr et j_rcvr */
	    if (j_rcvr > i_rcvr)
	      tpath2easy (i_rcvr, j_rcvr);

	    i_rcvr = 0;
	  }
	}
      }
      else {
	/* t ... */
	Tpq = -Xpq;
	/* ... et on met tous les terminaux */
	i = Tij2i (Tpq);
	j = Tij2j (Tpq);

	if (i >= 0 && j >= 0) {
	  XxYxZ_set (&easy_Tij_hd, i, Tpq, j, &trans); /* j > i */
	}
      }
    }
    return SXTRUE;
  }
  else {
    /* Aij */
    A = Aij2A (Aij); 

    if (SXBA_bit_is_set (easy_constituent_nt_set, A)) {
      /* C'est un constituant easy ...*/
      i = Aij2i (Aij);
      j = Aij2j (Aij);

      if (is_a_maximal_constituent (i, j)) {
	XxYxZ_set (&easy_hd, i, Aij, j, &triple);
      }

      /* Petit debug initial ...
	 printf ("%s[%i..%i]\n", spf.inputG.ntstring [A], Aij2i (Aij), Aij2j (Aij)); */
    }
  }
    
  return SXTRUE;
}

/* Il faut reconstituer les AMALGAM et les COMPOUND */
static SXINT
easy_print_Tij_stack (F)
     SXINT F;
{
  SXINT  top, x, y, z, tok_no, if_id, am_id, ste, href_id, Tij, Tij2;
  char *str1, *str2, easy_int [8], easy_string [16];

  top = easy_Tij_stack [0];

  for (x = 1; x <= top; x++) {
    Tij = easy_Tij_stack [x];
    tok_no = Tij2tok_no [Tij];
    if_id = toks_buf [tok_no].lahead; /* L'if_id a ete stocke en lahead par store_dag() ds dag_scanner.c */

    if (X_is_set (&X_amalgam_if_id_hd, if_id))
      /* C'est un element d'un amalgam */
      easy_Tij_stack [x] = -Tij;
  }

  for (x = 1; x <= top; x++) {
    Tij = easy_Tij_stack [x];
    am_id = 0;

    if (Tij < 0) {
      /* On cherche l'amalgam le + long!! */
      Tij = -Tij;
      tok_no = Tij2tok_no [Tij];
      z = top;

      while (x <= z) {
	XH_push (amalgam_list, toks_buf [tok_no].lahead);

	for (y = x+1; y <= z; y++) {
	  if ((Tij2 = easy_Tij_stack [y]) > 0)
	    break;
	
	  XH_push (amalgam_list, toks_buf [Tij2tok_no [-Tij2]].lahead);
	}

	if (am_id = XH_is_set (&amalgam_list)) {
	  /* il existe */
	  if (is_print_f_structure) printf ("F%i = \"%s\"", F, amalgam_names [am_id]);

	  if (xml_file) {
	    /* Pour chaque Xpq, on sauve le "href" correspondant */
	    sprintf (easy_string, "E%iF%i", sentence_id, F);
	    href_id = sxword_save (&easy_hrefs, easy_string);

	    /* On force tous les constituants de l'amalgame Tij a referencer href_id */
	    Xpq2easy_href [spf.outputG.maxxnt + Tij] = href_id;

	    while (++x < y) {
	      Xpq2easy_href [spf.outputG.maxxnt - easy_Tij_stack [x]] = href_id;
	    }

	    fprintf (xml_file,
		     " <F id=\"%s\">%s</F>\n",
		     easy_string,
		     amalgam_names [am_id]
		     );

	    sprintf (easy_int, "%i", F);
	    easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
	  }

	  x = y-1;
	  break;
	}

	z = y-2;
      }
    }
    else
      tok_no = Tij2tok_no [Tij];
    
    if (am_id == 0) {
      str1 = sxstrget (toks_buf [tok_no].string_table_entry);

      /* Les composants d'un COMPOUND sont separes par des '_' */
      while (str2 = strchr (str1, '_')) {
	*str2 = SXNUL;
	if (is_print_f_structure) printf ("F%i = \"%s\", ", F, str1);

	if (xml_file) {
	  fprintf (xml_file,
		   " <F id=\"E%iF%i\">%s</F>\n",
		   sentence_id,
		   F,
		   str1
		   );

	  sprintf (easy_int, "%i", F);
	  easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
	}

	*str2 = '_';
	str1 = str2+1;
	F++;
      }

      if (str2 = strchr (str1, '/'))
	/* On supprime le suffixe introduit par un '/' Ex le/det/ */
	*str2 = SXNUL;

      if (is_print_f_structure) printf ("F%i = \"%s\"", F, str1);
	  
      if (xml_file) {
	/* on sauve le "href" correspondant au dernier composant d'un mot compose' */
	sprintf (easy_string, "E%iF%i", sentence_id, F);
	Xpq2easy_href [spf.outputG.maxxnt + Tij] = sxword_save (&easy_hrefs, easy_string);

	fprintf (xml_file,
		 " <F id=\"%s\">%s</F>\n",
		 easy_string,
		 str1
		 );

	sprintf (easy_int, "%i", F);
	easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
      }

      if (str2)
	*str2 = '/';
    }

    F++;

    if (x < top)
      if (is_print_f_structure) fputs (", ", stdout);
  }

  return F;
}

static SXINT
easy_print_ste_path (F, i, j)
     SXINT F, i, j;
{
  SXINT triple, k, Tik, new_F;

  if (i == j) {
    F = easy_print_Tij_stack (F);
  }
  else {
    XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
      Tik = XxYxZ_Y (easy_Tij_hd, triple);

      k = XxYxZ_Z (easy_Tij_hd, triple);
	
      if (k <= j) {
	/* transition possible */
	PUSH (easy_Tij_stack, Tik);
	new_F = easy_print_ste_path (F, k, j);
	POP (easy_Tij_stack);

	if (new_F > F) {
	  /* un seul chemin !! */
	  F = new_F;
	  break;
	}
      }
    }
  }
    
  return F;
}

/* easy_constituent_stack contient un chemin complet constituants+terminaux de liaison */
static void
easy_print_constituent_path ()
{
  SXINT top, x, A, i, j, F, G;
  char easy_name [4], *str, easy_int [8], easy_string [16];

  fputs ("\n", stdout);

  easy_name [2] = SXNUL;

  top = easy_constituent_stack [0];
  F = G = 1;

  for (x = 1; x <= top; x++) {
    Aij = easy_constituent_stack [x];

    if (Aij <= spf.outputG.maxxnt) {
      A = Aij2A (Aij);
      i = Aij2i (Aij);
      j = Aij2j (Aij);

      strncpy (easy_name, spf.inputG.ntstring [A]+5, 2);
      if (is_print_f_structure) printf ("G%i : %s%i = [", G, easy_name, G);

      if (i >= 0 && j >= 0) {
	/* Pas des rcvr */
	/* On cherche un chemin terminal allant de i a j */
	/* dont les formes flechies sont reperees a partir de F */
	if (xml_file) {
	  sprintf (easy_string, "E%iG%i", sentence_id, G);
	  /* Pour chaque Xpq, on sauve le "href" correspondant */
	  Xpq2easy_href [Aij] = sxword_save (&easy_hrefs, easy_string);

	  fprintf (xml_file,
		   "<Groupe type=\"%s\" id=\"%s\">\n",
		   easy_name,
		   easy_string
		   );

	  sprintf (easy_int, "%i", G);
	  easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, easy_name), easy_int), " ");
	  
	  G++; 
	}
	
	F = easy_print_ste_path (F, i, j);
      
	if (is_print_f_structure) fputs ("]\n", stdout);

	if (xml_file)
	  fputs ("</Groupe>\n", xml_file);
      } 
    }
    else {
      PUSH (easy_Tij_stack, Aij - spf.outputG.maxxnt);
      /* Il faut traiter les amalgam et compound */
      F = easy_print_Tij_stack (F);
      if (is_print_f_structure) fputs ("\n", stdout);
      POP (easy_Tij_stack);
#if 0
      str = sxstrget (toks_buf [Tij2tok_no [Aij - spf.outputG.maxxnt]].string_table_entry);
      if (is_print_f_structure) printf ("     F%i = \"%s\"\n", F, str);

      if (xml_file) {
	/* on sauve le "href" correspondant a ce terminal de liaison (au cas ou .... ) */
	sprintf (easy_string, "E%iF%i", sentence_id, F);
	Xpq2easy_href [Aij] = sxword_save (&easy_hrefs, easy_string);

	fprintf (xml_file,
		 " <F id=\"%s\">%s</F>\n",
		 easy_string,
		 str
		 );

	sprintf (easy_int, "%i", F);
	easy_vstr = varstr_catenate (varstr_catenate (varstr_catenate (easy_vstr, "F"), easy_int), " ");
      }
		   
      F++;
#endif /* 0 */
    }
  }
}

static void
easy_header ()
{
  fputs ("<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n", xml_file);
    
  fprintf (xml_file,
	   "\n\
<!-- ********************************************************************\n\
\tThe file \"%s\" contains the XML format of the input sentence #%i\n",
	   xml_file_name,
	   sentence_id
	   );

  if (input_sentence_string)
    fprintf (xml_file,
	     "\t\"%s\"\n",
	     input_sentence_string
	     );

  fprintf (xml_file,
	   "\twhich has been generated\
\tby the SYNTAX(*) LFG processor SXLFG(*)\n\
     ********************************************************************\n\
\t(*) SYNTAX and SXLFG are trademarks of INRIA.\n\
     ******************************************************************** -->\n\n"
	   );

  fprintf (xml_file,
	   "<DOCUMENT fichier=\"%s\">\n",
	   xml_file_name);

  fprintf (xml_file,
	   "<E id=\"E%i\">\n",
	   sentence_id);
}

/* parcourt les productions referencees par un champ aij de valeur aij_val.
   sur chacune de ces productions Pij, elle appelle f(Pij).  Si F(Pij) retourne une valeur non nulle,
   le parcourt est abandonne' et walk_aij retourne cette valeur */
static SXINT
walk_aij (aij_val, f)
     SXINT aij_val;
     SXINT (*f) ();
{
  SXINT cur, top, Pij, ret_val;

  for (cur =  XH_X (XH_ufs_hd, aij_val), top = XH_X (XH_ufs_hd, aij_val+1);
      cur < top;
      cur++) {
    Pij = XH_list_elem (XH_ufs_hd, cur);

    if (Pij <= spf.outputG.maxxnt)
      sxtrap (ME, "walk_aij");

    Pij -= spf.outputG.maxxnt;

    if (ret_val = (*f)(Pij))
      return ret_val;
  }

  return 0;
}

/* retourne le "dpv_ptr" du champ pred de la f_structure fs_id */
static SXINT*
seek_pred (fs_id)
     SXINT fs_id;
{
  SXINT  val;
  SXINT  *val_ptr;
      
  if (fs_is_set (fs_id, PRED_ID, &val_ptr) && (val = (*val_ptr) >> FS_ID_SHIFT))
    return &(XH_list_elem (dynam_pred_vals, XH_X (dynam_pred_vals, val)));

  return NULL;
}

static SXINT
seek_Sujet_Verbe_Verbe (Pij)
     SXINT Pij;
{
  SXINT Aij, A, item, Xpq, href_verbe;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  A = Aij2A (Aij);
  str = spf.inputG.ntstring [A];

  if (strlen (str) > 9 && strncmp (str, "Easy_Infl", 9) == 0) {
    /* ... C'est la bonne */
    item = spf.outputG.lhs [Pij].prolon;
    
    while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
      if ((Xpq < 0) && (href_verbe = Xpq2easy_href [spf.outputG.maxxnt - Xpq])) {
	/* On suppose que c'est le bon "verbe" */
	return href_verbe;
      }
    }
  }
  
  return 0;
}


static SXINT
get_cln (Tpq)
     SXINT Tpq;
{
  SXINT  T;
  char *str;

  if (Tpq) {
    T = -Tij2T (Tpq-spf.outputG.maxxnt);
    str = spf.inputG.tstring [T];

    if (strlen (str) == 3 && strncmp (str, "cln", 3) == 0) {
      return Xpq2easy_href [Tpq];
    }
  }

  return 0;
}

static SXINT
get_cla (Tpq)
     SXINT Tpq;
{
  SXINT  T;
  char *str;

  if (Tpq) {
    T = -Tij2T (Tpq-spf.outputG.maxxnt);
    str = spf.inputG.tstring [T];

    if (strlen (str) == 3 && strncmp (str, "cla", 3) == 0) {
      return Xpq2easy_href [Tpq];
    }
  }

  return 0;
}

static SXINT
get_cld (Tpq)
     SXINT Tpq;
{
  SXINT  T;
  char *str;

  if (Tpq) {
    T = -Tij2T (Tpq-spf.outputG.maxxnt);
    str = spf.inputG.tstring [T];

    if (strlen (str) == 3 && strncmp (str, "cld", 3) == 0) {
      return Xpq2easy_href [Tpq];
    }
  }

  return 0;
}

static SXINT
get_clg (Tpq)
     SXINT Tpq;
{
  SXINT  T;
  char *str;

  if (Tpq) {
    T = -Tij2T (Tpq-spf.outputG.maxxnt);
    str = spf.inputG.tstring [T];

    if (strlen (str) == 3 && strncmp (str, "clg", 3) == 0) {
      return Xpq2easy_href [Tpq];
    }
  }

  return 0;
}


#if 0
static SXINT
seek_cln (Pij)
     SXINT Pij;
{
  SXINT  item, Xpq, href_sujet;
  char *str;

  item = spf.outputG.lhs [Pij].prolon;
    
  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq < 0) {
      str = spf.inputG.tstring [-Tij2T (-Xpq)];

      if (strlen (str) == 3 && strncmp (str, "cln", 3) == 0) {
	if (href_sujet = Xpq2easy_href [spf.outputG.maxxnt - Xpq])
	  return href_sujet;
      }
    }
  }
  
  return 0;
}


static SXINT
seek_cla (Pij)
     SXINT Pij;
{
  SXINT  item, Xpq, href_sujet;
  char *str;

  item = spf.outputG.lhs [Pij].prolon;
    
  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq < 0) {
      str = spf.inputG.tstring [-Tij2T (-Xpq)];

      if (strlen (str) == 3 && strncmp (str, "cla", 3) == 0) {
	if (href_sujet = Xpq2easy_href [spf.outputG.maxxnt - Xpq])
	  return href_sujet;
      }
    }
  }
  
  return 0;
}


static SXINT
seek_cld (Pij)
     SXINT Pij;
{
  SXINT  item, Xpq, href_sujet;
  char *str;

  item = spf.outputG.lhs [Pij].prolon;
    
  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq < 0) {
      str = spf.inputG.tstring [-Tij2T (-Xpq)];

      if (strlen (str) == 3 && strncmp (str, "cld", 3) == 0) {
	if (href_sujet = Xpq2easy_href [spf.outputG.maxxnt - Xpq])
	  return href_sujet;
      }
    }
  }
  
  return 0;
}


static SXINT
seek_clg (Pij)
     SXINT Pij;
{
  SXINT  item, Xpq, href_sujet;
  char *str;

  item = spf.outputG.lhs [Pij].prolon;
    
  while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
    if (Xpq < 0) {
      str = spf.inputG.tstring [-Tij2T (-Xpq)];

      if (strlen (str) == 3 && strncmp (str, "clg", 3) == 0) {
	if (href_sujet = Xpq2easy_href [spf.outputG.maxxnt - Xpq])
	  return href_sujet;
      }
    }
  }
  
  return 0;
}
#endif /* 0 */


static SXINT
seek_GN (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_gn;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GN", 7) == 0) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


static SXINT
seek_GR (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_gn;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GR", 7) == 0) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


/* Recherche ds la sous-foret de racine Aij le premier (?) terminal dont le (prefixe du) nom est tstr */
static SXINT
seek_t (Aij, tstr, is_prefix)
     SXINT     Aij;
     char    *tstr;
     SXBOOLEAN is_prefix;
{
  SXINT  hook, prod, item, Xpq, ret_val, lstr, ltstr;
  char *str;

  hook = spf.outputG.lhs [spf.outputG.maxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  if (ret_val = seek_t (Xpq, tstr, is_prefix))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];
	  lstr = strlen (str);
	  ltstr = strlen (tstr);

	  if ((is_prefix ? (lstr >= ltstr) : (lstr == ltstr)) && strncmp (str, tstr, ltstr) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}


static SXINT
seek_VERB_v (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_verbe;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == 4 && strncmp (str, "VERB", 4) == 0) {
    if (href_verbe = seek_t (Aij, "v", SXFALSE /* Le terminal "v" */))
      return href_verbe;
  }
  
  return 0;
}


static SXINT
seek_N_nc (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_verbe;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == 1 && strncmp (str, "N", 1) == 0) {
    if (href_verbe = seek_t (Aij, "nc", SXFALSE /* Le terminal "v" */))
      return href_verbe;
  }
  
  return 0;
}


static SXINT
seek_WS_csu (Pij)
     SXINT Pij;
{
  SXINT  Aij, href;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) == 2 && strncmp (str, "WS", 2) == 0) {
    if (href = seek_t (Aij, "csu", SXFALSE /* Le terminal "csu" */))
      return href;
  }
  
  return 0;
}


static SXINT
seek_Easy_PV_prep (Pij)
     SXINT Pij;
{
  SXINT  Aij, href;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_PV", 7) == 0) {
    if (href = seek_t (Aij, "prep", SXFALSE /* Le terminal "prep" */))
      return href;
  }
  
  return 0;
}


static SXINT
seek_Easy_GA_adj (Pij)
     SXINT Pij;
{
  SXINT  Aij, href;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GA", 7) == 0) {
    if (href = seek_t (Aij, "adj", SXFALSE /* Le terminal "adj" */))
      return href;
  }
  
  return 0;
}


static SXINT
seek_Easy_GR_adv (Pij)
     SXINT Pij;
{
  SXINT  Aij, href;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GR", 7) == 0) {
    if (href = seek_t (Aij, "adv", SXFALSE /* Le terminal "adv" */))
      return href;
  }
  
  return 0;
}


static SXINT
seek_Easy_PP_prep (Pij)
     SXINT Pij;
{
  SXINT  Aij, href;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_PP", 7) == 0) {
    if (href = seek_t (Aij, "prep", SXFALSE /* Le terminal "prep" */))
      return href;
  }
  
  return 0;
}


static SXINT
seek_NV (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_gn;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_NV", 7) == 0) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}

static SXINT
seek_GA (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_gn;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GA", 7) == 0) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}

static SXINT
seek_GP (Pij)
     SXINT Pij;
{
  SXINT  Aij, href_gn;
  char *str;

  Aij = spf.outputG.lhs [Pij].lhs;
  str = spf.inputG.ntstring [Aij2A (Aij)];

  if (strlen (str) >= 7 && strncmp (str, "Easy_GP", 7) == 0) {
    if (href_gn = Xpq2easy_href [Aij])
      return href_gn;
  }
  
  return 0;
}


static SXINT R; /* Numero de la relation */

/*
 C.15 Table r?capitulative sur l'annotation des relations
La table suivante r?capitule les informations mentionn?es ci-dessus. Elle indique la nature des constituants qui peuvent ?tre arguments pour chaque relation.

A noter :

   1. un constituant (GN, NV, GP, GA, GR, PV) peut ?tre remplac? par la forme lui correspondant;
   2. il est ?vident que pour annoter les relations dont la source et la cible sont dans le m?me constituant, il est imp?ratif d'utiliser les formes;
   3. dans le cas particulier de proposition sans verbe, le NV peut ?tre remplac? par un syntagme d'une autre nature : ceci n'est pas pris en compte dans la grille suivante (voir D.1.VIII Phrases et propositions sans verbe)



	                  Argument 1	                  Argument 2	   Argument 3
1. Sujet-Verbe            GN, NV, un pronom clitique 	  NV 	           _
2. Auxiliaire-Verbe       NV                        	  NV       	   _
3. COD-Verbe              GN, NV, PV, un pronom clitique  NV, PV    	   _
4. Compl?ment-Verbe       GP, PV, un pronom clitique  	  NV, PV   	   _
5. Modifieur-Verbe        GN, NV, GR                	  NV, PV   	   _
6. Compl?menteur          une forme                 	  NV, GN, GA	   _
7. Attribut-Sujet/Objet	  GA, GN, GP, PV           	  NV, PV   	   s ou o (pour sujet ou objet)
8. Modifieur-Nom	  GA, GP, GN, NV, PV, GR   	  GN, GP   	   x (pour indiquer la propagation), rien sinon
9. Modifieur-Adjectif	  GA, GR, GP, PV          	  GA, adjectif     -
                                                          d un GN sans nom
10. Modifieur-Adverbe	  GR                      	  GR               _
11. Modifieur-Pr?position GR                     	  une pr?position  _
12. Coordination          une conjonction de coordination GA, GP, GN,      GA, GP, GN,
                          une virgule                     NV, PV, GR       NV, PV, GR
                          un point virgule
13. Apposition            GN                              GN               _
14. Juxtaposition         GA, GP, GN,                     GA, GP, GN,      -       
                          NV, PV, GR                      NV, PV, GR
*/

static SXINT
atom_id2local_atom_id (atom_id)
     SXINT atom_id;
{
  /* 1<<local_atom_id == atom_id */
  SXINT local_atom_id, byte;

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

/*
<relation xmlns:xlink="extended" type="SUJ-V" id="E1R1"> 
<sujet xmlns:xlink="locator" href="E1G2"/> 
<verbe xmlns:xlink="locator" href="E1F7"/> 
</relation> 
*/
static void
easy_Sujet_Verbe ()
{
  SXINT  fs_id, sujet_val, atom_id, href_verbe, aij_val, val, cur, top, Pij, Aij, A, item, Xpq, href_sujet, x; 
  SXINT  *dpv_ptr, *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un champ subj */
    if (fs_is_set (fs_id, subj_id, &val_ptr) && (sujet_val = (*val_ptr) >> FS_ID_SHIFT)) {
      /* Il y a un champ subj qui reference la [sous-]structure sujet_val */
      if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... et un champ cat ... */
	if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_v) {
	  /* ... qui reference un verbe */

	  /* On cherche ce verbe */
	  href_verbe = 0;

	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    href_verbe = walk_aij (aij_val, seek_Sujet_Verbe_Verbe);
	  }

	  if (href_verbe) {
	    /* ... maintenant, on cherche le sujet ... */
	    /* ... qui est un pronom (clitique nominatif) */
	    /* ... si la f_structure sujet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
	       le terminal 'cln' */
	    /* ... ou un GN */
	    /* ... si  la f_structure sujet_val a un aij qui reference un Easy_GN */
	    str = (dpv_ptr = seek_pred (sujet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

	    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0)
	      href_sujet = get_cln (dpv_ptr [4]);
	    else {
	      if (fs_is_set (sujet_val, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		href_sujet = walk_aij (aij_val, seek_GN);
	      }
	      else
		href_sujet = 0;
	    }

#if 0
	    if (fs_is_set (sujet_val, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      str = (dpv_ptr = seek_pred (sujet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

	      if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0)
		href_sujet = walk_aij (aij_val, seek_cln);
	      else
		href_sujet = walk_aij (aij_val, seek_GN);
	    }
	    else
	      href_sujet = 0;
#endif /* 0 */

	    if (href_sujet) {
	      /* On a une relation Sujet-Verbe */
	      fprintf (xml_file,
		       "<relation xmlns:xlink=\"extended\" type=\"SUJ-V\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       ); 
	      fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href_sujet),
		       SXWORD_get (easy_hrefs, href_verbe)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("SUJ-V (%s, %s)\n",
			SXWORD_get (easy_hrefs, href_sujet)+Ei_lgth,
			SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			);
	      }
	    }
	  }
	}
      }
    }
  }
}


static SXINT *NV_stack;
static SXINT
get_aux_href (Aij)
     SXINT Aij;
{
  /* Aij est un noeud "Easy_.*", on cherche son constituant associe a un terminal de nom "aux.*" */
  /* On cherche toutes les Aij-prods */
  SXINT hook, prod, item, Xpq, ret_val;
  char *str;

  hook = spf.outputG.lhs [spf.outputG.maxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  str = spf.inputG.ntstring [Aij2A (Xpq)];

	  if (strlen (str) > 5 && strncmp (str, "Easy_", 5) == 0 && (ret_val = get_aux_href (Xpq)))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];

	  if (strlen (str) > 3 && strncmp (str, "aux", 3) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}

static SXINT
get_v_href (Aij)
     SXINT Aij;
{
  /* Aij est un noeud EASY_NVi, on cherche son constituant associe au terminal de nom "v" */
  SXINT hook, prod, item, Xpq, ret_val;
  char *str;

  hook = spf.outputG.lhs [spf.outputG.maxprod+Aij].prolon;

  while ((prod = spf.outputG.rhs [hook++].lispro) != 0) {
    if (prod > 0) {
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	/* On parcourt la RHS de la prod instanciee */
	if (Xpq > 0) {
	  str = spf.inputG.ntstring [Aij2A (Xpq)];

	  if (strlen (str) > 5 && strncmp (str, "Easy_", 5) == 0 && (ret_val = get_aux_href (Xpq)))
	    return ret_val;
	}
	else {
	  str = spf.inputG.tstring [-Tij2T (-Xpq)];

	  if (strlen (str) == 1 && strncmp (str, "v", 1) == 0)
	    return Xpq2easy_href [spf.outputG.maxxnt - Xpq];
	}
      }
    }
  }

  return 0;
}

/*
<relation xmlns:xlink="extended" type="AUX-V" id="E1R3"> 
<auxiliaire xmlns:xlink="locator" href="E1F7"/> 
<verbe xmlns:xlink="locator" href="E1F9"/> 
</relation> 
*/
static SXBOOLEAN
easy_td_Auxiliaire_Verbe (Aij)
     SXINT Aij;
{
  SXINT prod, item, Xpq, href_aux, href2_aux, href3_aux, href_v;
  char *str;

  if (Aij < 0) {
    /* prod */
    prod = -Aij;
    Aij = spf.outputG.lhs [prod].lhs;
    str = spf.inputG.ntstring [Aij2A (Aij)];

    if (strlen (str) == 4 && strncmp (str, "VERB", 4) == 0) {
      /* Production qui defini VERB */
      NV_stack [0] = 0;
      item = spf.outputG.lhs [prod].prolon;

      while ((Xpq = spf.outputG.rhs [item++].lispro) != 0) {
	if (Xpq > 0) {
	  str = spf.inputG.ntstring [Aij2A (Xpq)];
    
	  if (strlen (str) > 7 && strncmp (str, "Easy_NV", 7) == 0) {
	    PUSH (NV_stack, Xpq);
	    PUSH (NV_stack, str [7] - '0');
	  }
	}
      }

      switch (NV_stack[0] / 2) {
      case 2: /* (2, 3) ou (4, 5) */
	if (NV_stack [2] == 2 && NV_stack [4] == 3
	    || NV_stack [2] == 4 && NV_stack [4] == 5) {
	  href_aux = get_aux_href (NV_stack [1]);
	  href_v = get_v_href (NV_stack [3]);

	  if (href_aux && href_v) {
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href_aux),
		     SXWORD_get (easy_hrefs, href_v)
		     );
	    fputs ("</relation>\n", xml_file);

	    if (is_print_f_structure) {
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		      );
	    }
	  }
	}
	break;

      case 3: /* (6, 7, 5) ou (6, 8, 3) */
	if (NV_stack [2] == 6 && (NV_stack [4] == 7 && NV_stack [6] == 5
				  || NV_stack [4] == 8 && NV_stack [6] == 3)) {
	  href_aux = get_aux_href (NV_stack [1]);
	  href2_aux = get_aux_href (NV_stack [3]);
	  href_v = get_v_href (NV_stack [5]);

	  if (href_aux && href2_aux && href_v) {
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href_aux),
		     SXWORD_get (easy_hrefs, href2_aux)
		     );

	    fputs ("</relation>\n", xml_file);
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href2_aux),
		     SXWORD_get (easy_hrefs, href_v)
		     );
	    fputs ("</relation>\n", xml_file);

	    if (is_print_f_structure) {
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth
		      );
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		      );
	    }
	  }
	}
	break;

      case 4: /* (6, 8, 7, 5) */
	if (NV_stack [2] == 6 && NV_stack [4] == 8 && NV_stack [6] == 7 && NV_stack [8] == 5) {
	  href_aux = get_aux_href (NV_stack [1]);
	  href2_aux = get_aux_href (NV_stack [3]);
	  href3_aux = get_aux_href (NV_stack [5]);
	  href_v = get_v_href (NV_stack [7]);

	  if (href_aux && href2_aux && href3_aux && href_v) {
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href_aux),
		     SXWORD_get (easy_hrefs, href2_aux)
		     );
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href2_aux),
		     SXWORD_get (easy_hrefs, href3_aux)
		     );
	    fprintf (xml_file,
		     "<relation xmlns:xlink=\"extended\" type=\"AUX-V\" id=\"E%iR%i\">\n",
		     sentence_id,
		     ++R
		     ); 
	    /* On a une relation Sujet-Verbe */
	    fprintf (xml_file,"\
<sujet xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		     SXWORD_get (easy_hrefs, href3_aux),
		     SXWORD_get (easy_hrefs, href_v)
		     );

	    if (is_print_f_structure) {
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth
		      );
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href2_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href3_aux)+Ei_lgth
		      );
	      printf ("AUX-V (%s, %s)\n",
		      SXWORD_get (easy_hrefs, href3_aux)+Ei_lgth,
		      SXWORD_get (easy_hrefs, href_v)+Ei_lgth
		      );
	    }
	  }
	}

	break;

      default:
	break;
      }
    }
  }

  return SXTRUE;
}

static void
easy_Auxiliaire_Verbe ()
{
  /* On travaille uniquement ds la c_structure */
  spf.dag.pass_inherited = easy_td_Auxiliaire_Verbe;
  spf_topological_top_down_walk ();
}

/*
<relation xmlns:xlink="extended" type="COD-V" id="E2R3"> 
<cod xmlns:xlink="locator" href="E2G5"/> 
<verbe xmlns:xlink="locator" href="E2F7"/> 
</relation> 
*/
static void
easy_Objet_direct_Verbe ()
{  
  SXINT  fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, object_aij_val; 
  SXINT  *dpv_ptr, *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_v) {
	/* On cherche ce verbe */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT))
	  /* Y'a un champ aij */
	  href_verbe = walk_aij (aij_val, seek_VERB_v);
	else
	  aij_val = href_verbe = 0;
	
	if (href_verbe) {
	  /* href_verbe designe Le terminal "v" domine' par "VERB" */
	  /* ... et un champ obj */
	  if (fs_is_set (fs_id, obj_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... maintenant, on cherche l'objet ... */
	    /* ... qui est un pronom (clitique accusatif) */
	    /* ... si la f_structure objet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
	       le terminal 'cln' */
	    /* ... ou un GN */
	    /* ... si  la f_structure objet_val a un aij qui reference un Easy_GN */
	    str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

	    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0)
	      href_objet = get_cla (dpv_ptr [4]);
	    else {
	      if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		href_objet = walk_aij (object_aij_val, seek_GN);
	      }
	      else
		href_objet = 0;
	    }

#if 0
	    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

	      if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0)
		href_objet = walk_aij (object_aij_val, seek_cla);
	      else
		href_objet = walk_aij (object_aij_val, seek_GN);
	    }
	    else
	      href_objet = 0;
#endif /* 0 */

	    if (href_objet) {
	      /* On a une relation Objet direct-Verbe */
	      fprintf (xml_file,
		       "<relation xmlns:xlink=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       );
	      fprintf (xml_file,"\
<cod xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href_objet),
		       SXWORD_get (easy_hrefs, href_verbe)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("COD-V (%s, %s)\n",
			SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			);
	      }
	    }
	  }

	  /* ... un champ vcomp */
	  if (fs_is_set (fs_id, vcomp_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... maintenant, on cherche l'objet ... */
	    /* ... qui est un NV */
	    /* ... si  la f_structure objet_val a un aij qui reference un Easy_NV */
	    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      href_objet = walk_aij (object_aij_val, seek_NV);
	    }
	    else
	      href_objet = 0;

	    if (href_objet) {
	      /* On a une relation Objet direct-Verbe */
	      fprintf (xml_file,
		       "<relation xmlns:xlink=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       );
	      fprintf (xml_file,"\
<cod xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href_objet),
		       SXWORD_get (easy_hrefs, href_verbe)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("COD-V (%s, %s)\n",
			SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			);
	      }
	    }
	  }
	}

	/* ... un champ scomp */
	if (fs_is_set (fs_id, scomp_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* On cherche ce verbe */
	  if (aij_val)
	    /* Y'a un champ aij */
	    /* ... qui reference un Easy_NV */
	    href_verbe = walk_aij (aij_val, seek_NV);
	  else
	    href_verbe = 0;

	  if (href_verbe) {
	    /* href_verbe designe Le terminal "v" domine' par "VERB" */
	    /* ... maintenant, on cherche l'objet ... */
	    /* ... qui est un NV */
	    /* ... si  la f_structure objet_val a un aij qui reference un Easy_NV */
	    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      href_objet = walk_aij (object_aij_val, seek_NV);
	    }
	    else
	      href_objet = 0;

	    if (href_objet) {
	      /* On a une relation Objet direct-Verbe */
	      fprintf (xml_file,
		       "<relation xmlns:xlink=\"extended\" type=\"COD-V\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       );
	      fprintf (xml_file,"\
<cod xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href_objet),
		       SXWORD_get (easy_hrefs, href_verbe)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("COD-V (%s, %s)\n",
			SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			);
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="CPL-V" id="E1R5"> 
<complement xmlns:xlink="locator" href="E1G1"/> 
<verbe xmlns:xlink="locator" href="E1F9"/> 
</relation> 
*/
static void
easy_Complement_Verbe ()
{  
  SXINT  fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, i, cur, top, adjunct_val, object_aij_val, adjunct_aij_val; 
  SXINT  *dpv_ptr, *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_v) {
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* On cherche le mot "v" domine' par "VERB" */
	  if (href_verbe = walk_aij (aij_val, seek_VERB_v)) {
	    /* ... trouve' */
	    /* ... et un champ ".*-obj" */
	    for (i = 0; i <= 11; i++) {
	      if (fs_is_set (fs_id, obj_ids [i], &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* ... trouve' */
		/* ... maintenant, on cherche le complement ... */
		/* ... qui est un pronom (clitique datif ou genitif) */
		/* ... si la f_structure objet_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
		   le terminal 'cld' ou 'clg' */
		/* ... ou un GP */
		/* ... si  la f_structure objet_val a un aij qui reference un Easy_GP */
		str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
		  if ((href_objet = get_cld (dpv_ptr [4])) == 0)
		    href_objet = get_clg (dpv_ptr [4]);
		}
		else {
		  if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		    href_objet = walk_aij (object_aij_val, seek_GP);
		  }
		  else
		    href_objet = 0;
		}

#if 0
		if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		  if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
		    if ((href_objet = walk_aij (object_aij_val, seek_cld)) == 0) {
		      href_objet = walk_aij (object_aij_val, seek_clg);
		    }
		  }
		  else
		    href_objet = walk_aij (object_aij_val, seek_GP);
		}
		else
		  href_objet = 0;
#endif/* 0 */

		if (href_objet) {
		  /* On a une relation Complement_Verbe */
		  fprintf (xml_file,
			   "<relation xmlns:xlink=\"extended\" type=\"CPL-V\" id=\"E%iR%i\">\n",
			   sentence_id,
			   ++R
			   );
		  fprintf (xml_file,"\
<complement xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			   SXWORD_get (easy_hrefs, href_objet),
			   SXWORD_get (easy_hrefs, href_verbe)
			   );
		  fputs ("</relation>\n", xml_file);

		  if (is_print_f_structure) {
		    printf ("CPL-V (%s, %s)\n",
			    SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			    SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			    );
		  }
		}
	      }
	    }

	    for (i = 0; i <= 2; i++) {
	      if (fs_is_set (fs_id, vcomp_ids [i], &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* ... et un champ ".*-obj" */
		/* ... maintenant, on cherche le complement ... */
		/* ... qui est un NV */
		/* ... si  la f_structure objet_val a un aij qui reference un Easy_NV */
		if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  href_objet = walk_aij (object_aij_val, seek_NV);
		}
		else
		  href_objet = 0;

		if (href_objet) {
		  /* On a une relation Complement_Verbe */
		  fprintf (xml_file,
			   "<relation xmlns:xlink=\"extended\" type=\"CPL-V\" id=\"E%iR%i\">\n",
			   sentence_id,
			   ++R
			   );
		  fprintf (xml_file,"\
<complement xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			   SXWORD_get (easy_hrefs, href_objet),
			   SXWORD_get (easy_hrefs, href_verbe)
			   );
		  fputs ("</relation>\n", xml_file);

		  if (is_print_f_structure) {
		    printf ("CPL-V (%s, %s)\n",
			    SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			    SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			    );
		  }
		}
	      }
	    }

	    if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* ... et un champ "adjunct" */
	      for (cur = XH_X (XH_ufs_hd, objet_val), top = XH_X (XH_ufs_hd, objet_val+1);
		   cur < top;
		   cur++) {
		adjunct_val = XH_list_elem (XH_ufs_hd, cur);

		/* On cherche si adjunct_val contient le trait "pcas = y" */
		if (fs_is_set (adjunct_val, pcas_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ "pcas", peu importe sa valeur */
		  /* ... maintenant, on cherche le complement ... */
		  /* ... qui est un pronom (clitique datif ou genitif) */
		  /* ... si la f_structure adjunct_val a un predicat 'pro' et un aij qui reference une prod dont un fils est
		     le terminal 'cld' ou 'clg' */
		  /* ... ou un GP */
		  /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GP */
		  str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		  if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
		    if ((href_objet = get_cld (dpv_ptr [4])) == 0)
		      href_objet = get_clg (dpv_ptr [4]);
		  }
		  else {
		    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		      href_objet = walk_aij (object_aij_val, seek_GP);
		    }
		    else
		      href_objet = 0;
		  }

#if 0
		  if (fs_is_set (adjunct_val, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		    str = (dpv_ptr = seek_pred (objet_val)) ? lexeme_id2string [dpv_ptr [0] /* lexeme_id */] : NULL;

		    if (str && strlen (str) == 3 && strncmp (str, "pro", 3) == 0) {
		      if ((href_objet = walk_aij (adjunct_aij_val, seek_cld)) == 0) {
			href_objet = walk_aij (adjunct_aij_val, seek_clg);
		      }
		    }
		    else
		      href_objet = walk_aij (adjunct_aij_val, seek_GP);
		  }
		  else
		    href_objet = 0;
#endif /* 0 */

		  if (href_objet) {
		    /* On a une relation Complement_Verbe */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"CPL-V\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<complement xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href_objet),
			     SXWORD_get (easy_hrefs, href_verbe)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("CPL-V (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="MOD-V" id="E2R5"> 
<modifieur xmlns:xlink="locator" href="E2F6"/> 
<verbe xmlns:xlink="locator" href="E2F7"/> 
</relation> 
*/
static void
easy_Modifieur_Verbe ()
{
  SXINT  fs_id, objet_val, atom_id, x, aij_val, href_verbe, href_objet, i, cur, top, adjunct_val, object_aij_val; 
  SXINT  *dpv_ptr, *val_ptr;
  char *str;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_v) {
	/* ... oui */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij */
	  /* On cherche le mot "v" domine' par "VERB" */
	  if (href_verbe = walk_aij (aij_val, seek_VERB_v)) {
	    /* ... trouve' */
	    /* ... et un champ "adjunct" */
	    if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* ... trouve' */
	      for (cur = XH_X (XH_ufs_hd, objet_val), top = XH_X (XH_ufs_hd, objet_val+1);
		   cur < top;
		   cur++) {
		adjunct_val = XH_list_elem (XH_ufs_hd, cur);

		if (fs_is_set (adjunct_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  /* On cherche si adjunct_val NE CONTIENT PAS le trait "pcas = y" */
		  if (!fs_is_set (adjunct_val, pcas_id, &val_ptr) /* pas de champ pcas */
		      || ((*val_ptr) >> FS_ID_SHIFT) == 0 /* ou vide !! */
		      ) {
		    /* ... oui */
		    /* ... maintenant, on cherche le modifieur ... */
		    /* ... qui est un GN */
		    /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GN */
		    if (href_objet = walk_aij (object_aij_val, seek_GN)) {
		      /* On a une relation Modifieur_Verbe */
		      fprintf (xml_file,
			       "<relation xmlns:xlink=\"extended\" type=\"MOD-V\" id=\"E%iR%i\">\n",
			       sentence_id,
			       ++R
			       );
		      fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			       SXWORD_get (easy_hrefs, href_objet),
			       SXWORD_get (easy_hrefs, href_verbe)
			       );
		      fputs ("</relation>\n", xml_file);

		      if (is_print_f_structure) {
			printf ("MOD-V (%s, %s)\n",
				SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
				SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
				);
		      }
		    }
		  }

		  /* On cherche si adjunct_val contient le trait "cat = adv" */
		  if (fs_is_set (adjunct_val, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
		    if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_adv) {
		      /* ... oui */
		      /* ... maintenant, on cherche le modifieur ... */
		      /* ... qui est un GR */
		      /* ... si  la f_structure adjunct_val a un aij qui reference un Easy_GR */
		      if (href_objet = walk_aij (object_aij_val, seek_GR)) {
			/* On a une relation Modifieur_Verbe */
			fprintf (xml_file,
				 "<relation xmlns:xlink=\"extended\" type=\"MOD-V\" id=\"E%iR%i\">\n",
				 sentence_id,
				 ++R
				 );
			fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
				 SXWORD_get (easy_hrefs, href_objet),
				 SXWORD_get (easy_hrefs, href_verbe)
				 );
			fputs ("</relation>\n", xml_file);

			if (is_print_f_structure) {
			  printf ("MOD-V (%s, %s)\n",
				  SXWORD_get (easy_hrefs, href_objet)+Ei_lgth,
				  SXWORD_get (easy_hrefs, href_verbe)+Ei_lgth
				  );
			}
		      }
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="COMP" id="E3R4"> 
<complementeur xmlns:xlink="locator" href="E3F5"/> 
<verbe xmlns:xlink="locator" href="E3G3"/> 
</relation> 
*/
static void
easy_Complementeur ()
{
  SXINT x, fs_id, i, objet_val, aij_val, object_aij_val, href1, href2;
  SXINT *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* On cherche un champ ".*scomp" */
    for (i = 0; i <= 2; i++) {
      if (fs_is_set (fs_id, scomp_ids [i], &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... trouve' */
	if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij ds objet_val */
	  /* On cherche le mot "csu" domine' par "WS" */
	  if (href1 = walk_aij (object_aij_val, seek_WS_csu)) {
	    /* ... trouve' */
	    /* ... Easy_NV.* */
	    if (href2 = walk_aij (object_aij_val, seek_NV)) {
	      /* On a une relation Complementeur */
	      fprintf (xml_file,
		       "<relation xmlns:xlink=\"extended\" type=\"COMP\" id=\"E%iR%i\">\n",
		       sentence_id,
		       ++R
		       );
	      fprintf (xml_file,"\
<complementeur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
		       SXWORD_get (easy_hrefs, href1),
		       SXWORD_get (easy_hrefs, href2)
		       );
	      fputs ("</relation>\n", xml_file);

	      if (is_print_f_structure) {
		printf ("COMP (%s, %s)\n",
			SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			SXWORD_get (easy_hrefs, href2)+Ei_lgth
			);
	      }
	    }
	  }
	}
      }
    }

    /* On cherche un champ ".*vcomp" */
    for (i = 0; i <= 2; i++) {
      if (fs_is_set (fs_id, vcomp_ids [i], &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	/* ... trouve' */
	if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* Y'a un champ aij ds la structure principale */
	  /* On cherche le mot "prep" domine' par "Easy_PV" */
	  if (href1 = walk_aij (aij_val, seek_Easy_PV_prep)) {
	    /* ... trouve' */
	    if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* Y'a un champ aij ds objet_val */
	      /* On y cherche Easy_NV.* */
	      if (href2 = walk_aij (object_aij_val, seek_NV)) {
		/* On a une relation Complementeur */
		fprintf (xml_file,
			 "<relation xmlns:xlink=\"extended\" type=\"COMP\" id=\"E%iR%i\">\n",
			 sentence_id,
			 ++R
			 );
		fprintf (xml_file,"\
<complementeur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			 SXWORD_get (easy_hrefs, href1),
			 SXWORD_get (easy_hrefs, href2)
			 );
		fputs ("</relation>\n", xml_file);

		if (is_print_f_structure) {
		  printf ("COMP (%s, %s)\n",
			  SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			  SXWORD_get (easy_hrefs, href2)+Ei_lgth
			  );
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="ATB-SO" id="E6R30"> 
<attribut xmlns:xlink="locator" href="E6G31"/> 
<verbe xmlns:xlink="locator" href="E6G29"/> 
<s-o xmlns:xlink="locator" href="sujet"/> 
</relation> 
*/
static void
easy_Attribut_Sujet_objet ()
{
  SXINT  fs_id, objet_val, atom_id, x, aij_val, href1, href2, href3, i, cur, top, subj_val, val, object_aij_val; 
  SXINT  *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = v" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_v) {
	/* ... oui */
	/* ... et un champ ".*acomp" */
	for (i = 0; i <= 2; i++) {
	  if (fs_is_set (fs_id, acomp_ids [i], &val_ptr) && (objet_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* ... oui */
	    if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	      /* Y'a un champ aij ds la structure principale */
	      if (fs_is_set (objet_val, aij_id, &val_ptr) && (object_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		/* Y'a un champ aij ds objet_val */
		if (acomp_ids [i] == acomp_id && (href1 = walk_aij (object_aij_val, seek_GA)) ||
		    acomp_ids [i] != acomp_id /* de_acomp ou pour_acomp */&& (href1 = walk_aij (object_aij_val, seek_GP))) {
		  /* Easy_GA.* pointe' par acomp ou Easy_GP pointes par de_acomp ou pour_acomp */
		  if (href2 = walk_aij (object_aij_val, seek_NV)) {
		    /* Easy_NV.* pointe' par la principale */
		    /* On regarde si acomp contient un champ "subj" indice' */
		    href3 = 0;

		    if (fs_is_set (objet_val, subj_id, &val_ptr) && (subj_val = (*val_ptr) >> FS_ID_SHIFT)) {
		      /* Y'a un champ subj */
		      if (XH_list_elem (XH_fs_hd, XH_X (XH_fs_hd, subj_val+1)-1) < 0) {
			/* subj est partagee */
			/* On regarde s'il y a un subj ou un obj local partage' avec subj_val */
			if (fs_is_set (fs_id, subj_id, &val_ptr) && (val = (*val_ptr) >> FS_ID_SHIFT)) {
			  /* Y'a un subj local */
			  if (val == subj_val)
			    /* partage' */
			    href3 = 1; /* sujet */
			  else {
			    if (fs_is_set (fs_id, obj_id, &val_ptr) && (val = (*val_ptr) >> FS_ID_SHIFT)) {
			      /* Y'a un obj local */
			      if (val == subj_val)
				/* partage' */
				href3 = -1; /* objet */
			    }
			  }
			}
		      }
		    }
		
		    /* On a une relation Attribut-Sujet/Objet */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"ATB-SO\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<attribut xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<verbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );

		    if (href3)
		      fprintf (xml_file,"\
<s-o xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			       (href3 == 1) ? "sujet" : "objet"
			       );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("ATB-SO (%s, %s%s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth,
			      href3 ? ((href3 == 1) ? ", sujet" : ", objet") : ""
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*<relation xmlns:xlink="extended" type="MOD-N" id="E1R6"> 
<modifieur xmlns:xlink="locator" href="E1G7"/> 
<nom xmlns:xlink="locator" href="E1F12"/> 
<a-propager xmlns:xlink="locator" href="faux"/> 
</relation> 
*/
static void
easy_Modifieur_Nom ()
{
  SXINT x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item; 
  SXINT *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_nc) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_N_nc)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_GA)) || (href1 = walk_aij (adjunct_aij_val, seek_GP))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"MOD-N\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<nom xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
 
		    /* A VOIR */
		    fputs ("<a-propager xmlns:xlink=\"locator\" href=\"faux\"/>\n", xml_file);
  
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-N (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="MOD-A" id="E2R3"> 
<modifieur xmlns:xlink="locator" href="E2G3"/> 
<adjectif xmlns:xlink="locator" href="E2G2"/> 
</relation> 
*/
static void
easy_Modifieur_Adjectif ()
{
  SXINT x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item; 
  SXINT *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_adj) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_Easy_GA_adj)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_GR))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"MOD-A\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<adjectif xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-A (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="MOD-R" id="E6R43"> 
<modifieur xmlns:xlink="locator" href="E6G25"/> 
<adverbe xmlns:xlink="locator" href="E6G26"/> 
</relation> 
*/
static void
easy_Modifieur_Adverbe ()
{
  SXINT x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item; 
  SXINT *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_adv) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_Easy_GR_adv)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_GR))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"MOD-R\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<adverbe xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-R (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="MOD-P" id="E3R3"> 
<modifieur xmlns:xlink="locator" href="E3F3"/> 
<preposition xmlns:xlink="locator" href="E3F4"/> 
</relation> 
*/
static void
easy_Modifieur_Preposition ()
{
  SXINT x, fs_id, adjunct_val, atom_id, aij_val, adjunct_aij_val, cur, top, href1, href2, adjunct_item; 
  SXINT *val_ptr;

  /* Pour chaque f_structure ... */
  for (x = 1; x <= fs_id_stack [0]; x++) {
    fs_id = fs_id_stack [x];

    /* ... on regarde si elle a un trait local "cat = nc" ... */
    if (fs_is_set (fs_id, cat_id, &val_ptr) && (atom_id = (*val_ptr) >> FS_ID_SHIFT)) {
      if (field_id2atom_field_id [cat_id] [atom_id2local_atom_id (atom_id)] == atom_prep) {
	/* ... trouve' */
	/* ... et un champ "adjunct" */
	if (fs_is_set (fs_id, adjunct_id, &val_ptr) && (adjunct_val = (*val_ptr) >> FS_ID_SHIFT)) {
	  /* ... trouve' */
	  if (fs_is_set (fs_id, aij_id, &val_ptr) && (aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
	    /* Y'a un champ aij */
	    /* On cherche le mot "nc" domine' par "N" */
	    if (href2 = walk_aij (aij_val, seek_Easy_PP_prep)) {
	      /* On parcourt les adjunct ... */
	      for (cur = XH_X (XH_ufs_hd, adjunct_val), top = XH_X (XH_ufs_hd, adjunct_val+1);
		   cur < top;
		   cur++) {
		adjunct_item = XH_list_elem (XH_ufs_hd, cur);
		
		if (fs_is_set (adjunct_item, aij_id, &val_ptr) && (adjunct_aij_val = (*val_ptr) >> FS_ID_SHIFT)) {
		  /* Y'a un champ aij */
		  if ((href1 = walk_aij (adjunct_aij_val, seek_GR))) {
		    /* On a une relation Modifieur_Nom */
		    fprintf (xml_file,
			     "<relation xmlns:xlink=\"extended\" type=\"MOD-P\" id=\"E%iR%i\">\n",
			     sentence_id,
			     ++R
			     );
		    fprintf (xml_file,"\
<modifieur xmlns:xlink=\"locator\" href=\"%s\"/>\n\
<preposition xmlns:xlink=\"locator\" href=\"%s\"/>\n",
			     SXWORD_get (easy_hrefs, href1),
			     SXWORD_get (easy_hrefs, href2)
			     );
		    fputs ("</relation>\n", xml_file);

		    if (is_print_f_structure) {
		      printf ("MOD-P (%s, %s)\n",
			      SXWORD_get (easy_hrefs, href1)+Ei_lgth,
			      SXWORD_get (easy_hrefs, href2)+Ei_lgth
			      );
		    }
		  }
		}
	      }
	    }
	  }
	}
      }
    }
  }
}

/*
<relation xmlns:xlink="extended" type="COORD" id="E6R44"> 
<coordonnant xmlns:xlink="locator" href="E6F12"/> 
<coord-g xmlns:xlink="locator" href="E6G1"/> 
<coord-d xmlns:xlink="locator" href="E6G6"/> 
</relation> 
*/
static void
easy_Coordination ()
{
  fprintf (xml_file,
	   "<relation xmlns:xlink=\"extended\" type=\"COORD\" id=\"E%iR%i\">\n",
	   sentence_id,
	   ++R
	   );

  fputs ("</relation>\n", xml_file);
}

/*
<relation xmlns:xlink="extended" type="APPOS" id="E6R51"> 
<premier xmlns:xlink="locator" href="E6G9"/> 
<appose xmlns:xlink="locator" href="E6G6"/> 
</relation> 
*/
static void
easy_Apposition ()
{
  fprintf (xml_file,
	   "<relation xmlns:xlink=\"extended\" type=\"APPOS\" id=\"E%iR%i\">\n",
	   sentence_id,
	   ++R
	   );

  fputs ("</relation>\n", xml_file);
}

/*
<relation xmlns:xlink="extended" type="JUXT" id="E4R2"> 
<premier xmlns:xlink="locator" href="E4G1"/> 
<suivant xmlns:xlink="locator" href="E4G2"/> 
</relation> 
*/
static void
easy_Juxtaposition ()
{
  fprintf (xml_file,
	   "<relation xmlns:xlink=\"extended\" type=\"JUXT\" id=\"E%iR%i\">\n",
	   sentence_id,
	   ++R
	   );

  fputs ("</relation>\n", xml_file);
}


static void
easy_print_relations ()
{
  SXINT cur_nb, i, head, Xpq, fs_id, pos;

  fputs ("<relations>\n", xml_file);

  cur_nb = head_stack [0]; /* Nombre de structures "valides" */

  for (i = 1; i <= cur_nb; i++) {
    head = head_stack [i];
    Xpq = XxY_X (heads, head);
    fs_id = XxY_Y (heads, head);

    fill_fs_id_stack (fs_id);

    easy_Sujet_Verbe ();
    easy_Auxiliaire_Verbe ();
    easy_Objet_direct_Verbe ();
    easy_Complement_Verbe ();
    easy_Modifieur_Verbe ();
    easy_Complementeur ();
    easy_Attribut_Sujet_objet ();
    easy_Modifieur_Nom ();
    easy_Modifieur_Adjectif ();
    easy_Modifieur_Adverbe ();
    easy_Modifieur_Preposition ();
    easy_Coordination ();
    easy_Apposition ();
    easy_Juxtaposition ();
  }

  fputs ("</relations>\n", xml_file);
}


static SXINT easy_path_nb;
/* On parcourt les constituants consecutifs */
static void
easy_walk (i)
     SXINT i;
{
  SXINT     triple, Aij, j, Tij;
  SXBOOLEAN is_constituent_trans;

  if (i == final_mlstn) {
    /* easy_constituent_stack contient un chemin complet (constituants et terminaux de liaison) */
    if (++easy_path_nb == 1) {
      /* Je n'imprime que le 1er chemin ... */
      if (xml_file)
	fputs ("<constituants>\n", xml_file);

      easy_print_constituent_path ();

      if (xml_file) {
	fputs ("</constituants>\n", xml_file);
	fputs ("<liste_constituants> \n", xml_file);
	fprintf (xml_file,
	   "%s\n",
	   varstr_tostr (easy_vstr)); 
	fputs ("</liste_constituants>\n", xml_file);

	easy_print_relations ();

	fputs ("</E>\n", xml_file);
      }
    }

    return;
  } 

  is_constituent_trans = SXFALSE;

  XxYxZ_Xforeach (easy_hd, i, triple) {
    is_constituent_trans = SXTRUE;
    Aij =  XxYxZ_Y (easy_hd, triple);
    PUSH (easy_constituent_stack, Aij);
    j = XxYxZ_Z (easy_hd, triple);

    easy_walk (j);

    POP (easy_constituent_stack);
  }

  if (!is_constituent_trans) {
    /* On fait une transition terminale ... */
    XxYxZ_Xforeach (easy_Tij_hd, i, triple) {
      is_constituent_trans = SXTRUE;
      Tij =  XxYxZ_Y (easy_Tij_hd, triple);
      PUSH (easy_constituent_stack, spf.outputG.maxxnt+Tij);
      j = XxYxZ_Z (easy_Tij_hd, triple);

      easy_walk (j);

      POP (easy_constituent_stack);
    }

    if (!is_constituent_trans)
      /* Faut-il faire Tij_iforeach aussi ? */
      sxtrap (ME, "easy_walk");
  }
}

/* On repere les constituants */
static void
easy_constituent ()
{
  static SXINT      easy_hd_foreach [6] = {1, 0, 0, 0, 0, 0};
  static SXINT      easy_Tij_hd_foreach [6] = {1, 0, 0, 0, 0, 0};
  SXINT  nt, t, lgth;
  char *nt_string, *t_string;

  easy_constituent_nt_set = sxba_calloc (spf.inputG.maxnt+1);
  XxYxZ_alloc (&easy_hd, "easy_hd", final_mlstn+1, 1, easy_hd_foreach, NULL, NULL);
  XxYxZ_alloc (&easy_Tij_hd, "easy_Tij_hd", final_mlstn+1, 1, easy_Tij_hd_foreach, NULL, NULL);
  easy_constituent_stack = (SXINT*) sxalloc (final_mlstn+1, sizeof (SXINT)), easy_constituent_stack [0] = 0;
  easy_Tij_stack = (SXINT*) sxalloc (final_mlstn+1, sizeof (SXINT)), easy_Tij_stack [0] = 0;

  for (nt = 1; nt <= spf.inputG.maxnt; nt++) {
    nt_string = spf.inputG.ntstring [nt];

    if (strlen (nt_string) > 5 && strncmp (nt_string, "Easy_", 5) == 0) {
      /* C'est un easy ... */
      /* ... mais est-ce un constituant ? */
      if (strlen (nt_string+5) >= 2) {
	if (strncmp (nt_string+5, "GA", 2) == 0
	    ||strncmp (nt_string+5, "GN", 2) == 0
	    ||strncmp (nt_string+5, "GP", 2) == 0
	    ||strncmp (nt_string+5, "GR", 2) == 0
	    ||strncmp (nt_string+5, "PV", 2) == 0
	    ||strncmp (nt_string+5, "NV", 2) == 0
	    )
	  SXBA_1_bit (easy_constituent_nt_set, nt);
      }
    }
  }

#if 0
  /* Les "coo", "csu", "poncts", "ponctw" et "prep" (non compris ds des GP ou GV ) ne font pas
   partie des constituants Easy */
  easy_constituent_t_set = sxba_calloc (-spf.inputG.maxt+1);

  for (t = 1; t <= -spf.inputG.maxt; t++) {
    t_string = spf.inputG.tstring [t];
    lgth = strlen (t_string);

    if (lgth == 3) {
      if (strncmp (t_string, "coo", 3) == 0)
	SXBA_1_bit (easy_constituent_t_set, t);
      else
	if (strncmp (t_string, "csu", 3) == 0)
	  SXBA_1_bit (easy_constituent_t_set, t);
    }
    else {
      if (lgth == 4) {
	if (strncmp (t_string, "prep", 4) == 0)
	  SXBA_1_bit (easy_constituent_t_set, t);
      }
      else {
	if (lgth == 6) {
	  if (strncmp (t_string, "poncts", 6) == 0)
	    SXBA_1_bit (easy_constituent_t_set, t);
	  else
	    if (strncmp (t_string, "ponctw", 6) == 0)
	      SXBA_1_bit (easy_constituent_t_set, t);
	}
      }
    }
  }
#endif /* 0 */
  
  if (xml_file)
    easy_header ();

  if (spf.outputG.start_symbol) {
    max_constituent_set = sxba_calloc (n+1);

    spf.dag.pass_inherited = easy_td_walk;
    spf_topological_top_down_walk ();

    sxfree (max_constituent_set), max_constituent_set = NULL;

    /* Il faut chercher les compositions sequentielles */
    easy_walk (1);

    if (easy_path_nb > 1)
      /* Il y a easy_path_nb-1 autres chemins non sortis */
      printf ("This sentence can be split into constituents in %i other ways ...\n", easy_path_nb-1);
  }

  if (xml_file)
    fputs ("</DOCUMENT>\n", xml_file);

  sxfree (easy_constituent_nt_set), easy_constituent_nt_set = NULL;
#if 0
  sxfree (easy_constituent_t_set), easy_constituent_t_set = NULL;
#endif /* 0 */
  XxYxZ_free (&easy_hd);
  XxYxZ_free (&easy_Tij_hd);
  sxfree (easy_constituent_stack), easy_constituent_stack = NULL;
  sxfree (easy_Tij_stack), easy_Tij_stack = NULL;
}

static void
easy_campaign ()
{
  SXINT           id, top;
  sxword_header field_names, atom_names;

  /* On commence par calculer les id des champs utilises pour le calcul des relations */
  sxword_alloc (&field_names, "field_names", MAX_FIELD_ID+1, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

  for (id = 2; id <= MAX_FIELD_ID; id++) {
    sxword_save (&field_names, field_id2string [id]);
  }

  if ((subj_id = sxword_2retrieve (&field_names, "subj", 4)) == 0) sxtrap (ME, "easy_campaign");
  if ((cat_id = sxword_2retrieve (&field_names, "cat", 3)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_id = sxword_2retrieve (&field_names, "obj", 3)) == 0) sxtrap (ME, "easy_campaign");
  if ((aij_id = sxword_2retrieve (&field_names, "aij", 3)) == 0) sxtrap (ME, "easy_campaign");
  if ((vcomp_id = sxword_2retrieve (&field_names, "vcomp", 5)) == 0) sxtrap (ME, "easy_campaign");
  if ((scomp_id = sxword_2retrieve (&field_names, "scomp", 5)) == 0) sxtrap (ME, "easy_campaign");
  if ((acomp_id = sxword_2retrieve (&field_names, "acomp", 5)) == 0) sxtrap (ME, "easy_campaign");
  if ((pcas_id = sxword_2retrieve (&field_names, "pcas", 4)) == 0) sxtrap (ME, "easy_campaign");

  if ((obj_ids [0] = a_obj_id = sxword_2retrieve (&field_names, "à-obj", 5)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [1] = de_obj_id = sxword_2retrieve (&field_names, "de-obj", 6)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [2] = par_obj_id = sxword_2retrieve (&field_names, "par-obj", 7)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [3] = devant_obj_id = sxword_2retrieve (&field_names, "devant-obj", 10)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [4] = dans_obj_id = sxword_2retrieve (&field_names, "dans-obj", 8)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [5] = sur_obj_id = sxword_2retrieve (&field_names, "sur-obj", 7)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [6] = en_obj_id = sxword_2retrieve (&field_names, "en-obj", 6)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [7] = contre_obj_id = sxword_2retrieve (&field_names, "contre-obj", 10)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [8] = apres_obj_id = sxword_2retrieve (&field_names, "après-obj", 9)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [9] = avec_obj_id = sxword_2retrieve (&field_names, "avec-obj", 8)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [10] = vers_obj_id = sxword_2retrieve (&field_names, "vers-obj", 8)) == 0) sxtrap (ME, "easy_campaign");
  if ((obj_ids [11] = pour_obj_id = sxword_2retrieve (&field_names, "pour-obj", 8)) == 0) sxtrap (ME, "easy_campaign");

  if ((vcomp_ids [0] = a_vcomp_id = sxword_2retrieve (&field_names, "à-vcomp", 7)) == 0) sxtrap (ME, "easy_campaign");
  if ((vcomp_ids [1] = de_vcomp_id = sxword_2retrieve (&field_names, "de-vcomp", 8)) == 0) sxtrap (ME, "easy_campaign");
  if ((vcomp_ids [2] = par_vcomp_id = sxword_2retrieve (&field_names, "par-vcomp", 9)) == 0) sxtrap (ME, "easy_campaign");

  if ((scomp_ids [0] = a_scomp_id = sxword_2retrieve (&field_names, "à-scomp", 7)) == 0) sxtrap (ME, "easy_campaign");
  if ((scomp_ids [1] = de_scomp_id = sxword_2retrieve (&field_names, "de-scomp", 8)) == 0) sxtrap (ME, "easy_campaign");
  if ((scomp_ids [2] = scomp_id = sxword_2retrieve (&field_names, "scomp", 5)) == 0) sxtrap (ME, "easy_campaign");

  if ((acomp_ids [0] = acomp_id = sxword_2retrieve (&field_names, "acomp", 5)) == 0) sxtrap (ME, "easy_campaign");
  if ((acomp_ids [1] = de_acomp_id = sxword_2retrieve (&field_names, "de-acomp", 8)) == 0) /* sxtrap (ME, "easy_campaign") */;
  if ((acomp_ids [2] = pour_acomp_id = sxword_2retrieve (&field_names, "pour-acomp", 10)) == 0) /* sxtrap (ME, "easy_campaign") */;

  if ((adjunct_id = sxword_2retrieve (&field_names, "adjunct", 7)) == 0) sxtrap (ME, "easy_campaign");

  sxword_free (&field_names);

  top = sizeof (atom_id2string) / sizeof (char*);

  sxword_alloc (&atom_names, "atom_names", top+1, 1, 16,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);

  for (id = 2; id < top; id++) {
    sxword_save (&atom_names, atom_id2string [id]);
  }

  if ((atom_v = sxword_2retrieve (&atom_names, "v", 1)) == 0) sxtrap (ME, "easy_campaign");
  if ((atom_adv = sxword_2retrieve (&atom_names, "adv", 3)) == 0) sxtrap (ME, "easy_campaign");
  if ((atom_nc = sxword_2retrieve (&atom_names, "nc", 2)) == 0) sxtrap (ME, "easy_campaign");
  if ((atom_adj = sxword_2retrieve (&atom_names, "adj", 3)) == 0) sxtrap (ME, "easy_campaign");
  if ((atom_prep = sxword_2retrieve (&atom_names, "prep", 4)) == 0) /* sxtrap (ME, "easy_campaign") */;

  sxword_free (&atom_names);


  X_reuse (&X_amalgam_if_id_hd, "X_amalgam_if_id_hd", NULL, NULL);
  XH_unlock (amalgam_list);

  easy_vstr = varstr_alloc (128);
  sxword_alloc (&easy_hrefs, "easy_hrefs", Xpq_top+2, 1, 8,
		sxcont_malloc, sxcont_alloc, sxcont_realloc, NULL, sxcont_free, NULL, NULL);
  Xpq2easy_href = (SXINT*) sxcalloc (Xpq_top+2, sizeof (SXINT));
  NV_stack = (SXINT*) sxalloc (2*spf.inputG.maxrhs+1, sizeof (SXINT)); 

  if (is_print_f_structure) {
    char string [16];

    sprintf (string, "E%i", sentence_id);
    Ei_lgth = strlen (&(string [0]));
  }

  easy_constituent ();

  varstr_free (easy_vstr), easy_vstr = NULL;
  sxword_free (&easy_hrefs);
  sxfree (Xpq2easy_href), Xpq2easy_href = NULL;
  sxfree (NV_stack), NV_stack = NULL;
}
#endif /* EASY */


static SXINT
forest_sem_pass ()
{
  SXUINT tree_count;
  SXINT               x, cur_nb;
  SXINT               f_struct_nb, unconsistent_f_struct_nb, total_f_struct_nb;
  SXBOOLEAN           is_OK;

  if (LFG_TIME > spf.inputG.modif_time) {
    fputs ("\nThe Earley parser tables are older than the current LFG tables,\n\
run \"bnf\" and \"csynt_lc\" on the CF skeleton generated by the SXLFG constructor executed with the \"-bnf\" option.\n", sxstderr);
    sxexit (4);
  }

  /* On complete la structure "Tij" avec les terminaux couverts par les nt de la rcvr */
  t_completing ();

  if (is_print_f_structure) {
    is_OK = spf_tree_count (&tree_count);

    printf ("\n(|N|=%i, |T|=%i, |P|=%i, S=%i, |outpuG|=%i, TreeCount=", 
	     spf.outputG.maxxnt, maxxt, spf.outputG.maxprod, spf.outputG.start_symbol, spf.outputG.maxitem);

    if (is_OK || spf.outputG.start_symbol == 0)
      printf ("%lu)\n", tree_count);
    else
      fputs ("Cyclic)\n", stdout);
  }

  /* Dans tous les cas on commence par instancier les feuilles terminales.  C,a permet d'avoir un xml
     minimal qui contiendra les symboles terminaux et les f_structures associees */
  Xpq_top = spf.outputG.maxxnt + maxxt;
  allocate_all ();

  set_terminal_leaves ();

  set_non_terminal_error ();

  if (spf.outputG.start_symbol == 0) {
    fputs ("Parsing failed\n", sxtty);
  }
  else {
    if (spf.inputG.has_cycles) {
      fputs ("Sorry, cyclic grammars cannot be f_structured.\n", sxtty);
    }
    else {
      spf.dag.pass_derived = dag_bu_walk;
      spf_topological_bottom_up_walk ();
      
      if ((x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), head_stack [0] = 0;
	head_set = sxba_calloc (XxY_top (heads)+1);

	total_f_struct_nb = fs_id_dstack [x];
	unconsistent_f_struct_nb = sxba_cardinal (unconsistent_heads_set);
	f_struct_nb = total_f_struct_nb - unconsistent_f_struct_nb; /* Les coherentes */
	is_consistent = f_struct_nb > 0 || unconsistent_f_struct_nb == 0; /* static pour filter_root */
  
	/* On prend les f_structures de la racine :
	   - soit uniquement les coherentes (s'il y en a)
	   - soit la totalite' (desincoherentes) */
	fill_head_stack (spf.outputG.start_symbol, filter_root);
      }
      else {
	/* Echec de la 1ere passe */
	/* On reessaie sans faire les tests de coherence ... */
	is_relaxed_run = SXTRUE;
	is_locally_unconsistent = SXFALSE;
	clear_for_relaxed_run ();

#if EBUG      
	if (!is_print_f_structure) {
	  fputs ("\n\n\n<<<The first attempt failed>>>\n\
****************** S E C O N D   A T T E M P T ******************\n\
               (the consistency checks are relaxed)\n\n", stdout);
	}
#endif /* EBUG */
      
	if (is_print_f_structure) {
	  fputs ("\n\n\n<<<The first attempt failed>>>\n\
****************** S E C O N D   A T T E M P T ******************\n\
               (the consistency checks are relaxed)\n\n", stdout);
	}
      
	spf_topological_bottom_up_walk ();

	head_stack = (SXINT*) sxalloc (XxY_top (heads)+1, sizeof (SXINT)), head_stack [0] = 0;
	head_set = sxba_calloc (XxY_top (heads)+1);
      
	if ((x = Xpq2disp [spf.outputG.start_symbol]) > 0) {
	  /* On prend toutes les f_structures de la racine */
	  fill_head_stack (spf.outputG.start_symbol, filter_root);
	}
	else {
	  /* Echec : aucune f_structure a la racine */
	  /* On parcourt la foret partagee top_down jusqu'a rencontrer des noeuds qui contiennent des
	     f_structures qui seront sorties */
	  /* if (spf.outputG.start_symbol == 0 || spf.inputG.has_cycles) on sort les f_structures des terminaux */
	  spf.dag.pass_derived = rcvr_bu_walk;
	  spf_topological_bottom_up_walk ();
	  is_rcvr_done = SXTRUE;
	}
      }

      cur_nb = head_stack [0]; /* Nombre de structures "valides" */

      if (rank [0]) {
	/* f_structures multiples et ranking demande' */
	for (x = 1; x <= 4 && cur_nb > 1; x++) {
	  switch (rank [x]) {
	  case 0:
	    /* 0 : Rien ... */
	    break;

	  case 1:
	    /* 1 : Calcul des priorites des f_structures a partir des priorites des elements lexicaux */
	    process_priority ();
	    ranking_kind *= 10;
	    ranking_kind += 1;
	    break;

	  case 2:
	    /* 2 : preferer les arguments aux modifieurs */
	    ranking_by_args ();
	    ranking_kind *= 10;
	    ranking_kind += 2;
	    break;

	  case 3:
	    /* 3 : Preferer les arguments proches */
	    ranking_by_vicinity ();
	    ranking_kind *= 10;
	    ranking_kind += 3;
	    break;

	  case 4:
	    /* 4 : Preferer les structures les + enchassees */
	    ranking_by_embedding ();
	    ranking_kind *= 10;
	    ranking_kind += 4;
	    break;

	  case 5:
	    /* 5 : Preferer la valeurs atomique locale la plus faible du champ "special_atomic_field_id"
	       de la structure principale.  Pour l'instant special_atomic_field_id == "mode" ds la campagne easy */
	    ranking_by_special_atomic_field ();
	    ranking_kind *= 10;
	    ranking_kind += 5;
	    break;
	  }

	  cur_nb = head_stack [0]; /* Nombre de structures "valides" */
	}

#ifdef EASY
	/* On est ds la campagne d'evaluation EASY ... */
	if (cur_nb > 1) {
	  /* ... On ne conserve qu'une seule f_structure par noeud maximal (racine sinon noeuds maximaux internes) */
	  single_f_structure_filtering ();
	  cur_nb = head_stack [0]; /* Nombre de structures "valides" */
	}
#endif /* EASY */
      }

#if EBUG
      if (!is_print_f_structure) {
	printf ("%s consistency checks, there %s %i %sf_structure%s in this shared parse forest (ranking_kind=%i)\n",
		is_relaxed_run ? "Without" : "After",
		(cur_nb == 1) ? "is" : "are",
		cur_nb,
		is_rcvr_done ? "partial " : (is_relaxed_run ? "" : (is_consistent ? "consistent " : "unconsistent ")),
		(cur_nb == 1) ? "" : "s",
		ranking_kind);
      }
#endif /* EBUG */

      if (is_print_f_structure) {
	SXINT head, i;
	SXINT *base_ptr;

	printf ("\n*** %s consistency check, there %s %i %sf_structure%s in this shared parse forest (ranking_kind=%i) ***\n",
		is_relaxed_run ? "Without" : "After",
		(cur_nb == 1) ? "is" : "are",
		cur_nb,
		is_rcvr_done ? "partial " : (is_relaxed_run ? "" : (is_consistent ? "consistent " : "unconsistent ")),
		(cur_nb == 1) ? "" : "s",
		ranking_kind);

	if (cur_nb) {
	  /* Si !is_consistent, on imprime tout le monde, sinon, on n'imprique que les coherents */
	  for (i = 1; i <= head_stack [0]; i++) {
	    head = head_stack [i];

	    if (i > 1)
	      fputs ("\n", stdout);

	    printf ("#%i: ", i);
	    print_f_structure (XxY_X (heads, head) /* Xpq */, XxY_Y (heads, head) /* fs_id */, !is_consistent);
	  }
	}

	fputs ("*** End of the main f_structures ***\n\n", stdout);
      }

      if (!is_rcvr_done && is_forest_filtering && spf.outputG.maxnt == spf.outputG.maxxnt) {
	/* On reduit la foret syntaxique en fonctions des f_structures partagees */
	/* Sauf s'il n'y a pas de f_structure a la racine ... */
	/* ... ou il y a eu de la rcvr */
#if EBUG
	valid_fs_id_set = sxba_calloc (XH_top (XH_fs_hd)+1);
	unconsistent_fs_id_set = sxba_calloc (XH_top (XH_fs_hd)+1);
	fputs ("\n*** Top_down filtering of the shared parse forest ***", stdout);
#endif /* EBUG */

	valid_heads_set = sxba_calloc (XxY_top (heads)+1);

	spf.dag.pass_inherited = dag_td_walk;
	spf_topological_top_down_walk ();

	sxfree (valid_heads_set), valid_heads_set = NULL;

#if EBUG
	fputs ("*** End of the top_down filtering of the shared parse forest ***\n\n", stdout);
	sxfree (valid_fs_id_set), valid_fs_id_set = NULL;
	sxfree (unconsistent_fs_id_set), unconsistent_fs_id_set = NULL;
#endif /* EBUG */
      }
      /* else on sortira ds le xml la foret partagee en entier ... */
    }
  }

#ifdef EASY
  /* On est ds la campagne d'evaluation EASY */
  easy_campaign ();
#else
  if (xml_file)
    xml_output ();
#endif /* EASY */

  free_all ();

  return 1;
}

void
lfg_semact ()
{
  SXINT i;
  
  for_semact.sem_init = NULL;
  for_semact.sem_final = NULL;
  for_semact.semact = NULL;
  for_semact.parsact = NULL;
  for_semact.prdct = NULL;
  for_semact.constraint = NULL;
  for_semact.sem_pass = is_unfold_parse_forest ? tree_sem_pass : forest_sem_pass;

  if (dfield_pairs2field_id) {
    /* Y'a des noms de champs dynamiques, on reactive dfield_pairs */
    XxY_reuse (&dfield_pairs, "dfield_pairs", NULL /* oflw */, NULL /* stats */);
  }

  for (i = 0; i <= MAXRHS; i++)
    empty_shared_rhs.rhs [i] = -1;
}
