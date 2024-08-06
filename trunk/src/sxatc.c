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

/* Actions de construction d'arbre pour la nouvelle version de SYNTAX (TM) */

#define SXATC_AREA	struct sxatc_area	/* used in "sxunix.h" */

#include "sxversion.h"
#include "sxunix.h"
#include "sxba.h"

#include <memory.h>

char WHAT_SXATC[] = "@(#)SYNTAX - $Id: sxatc.c 4143 2024-08-02 08:50:12Z garavel $" WHAT_DEBUG;

struct sxatc_area {
    struct sxatc_area	*next;
    SXNODE		zone [1];
};

#define ERROR_n 1

/* Gestion dans le tas : */

static SXNODE	**stacktreenode /* [stns] */ ;
static SXINT	stns;

#define stns_incr	100

char * p_char_tmp;
/* this variable p_char_tmp is used in the following macro : allocate_old_node
   it is needed for pointer arithmetics compatible with gcc, sunCC and intelCC */

#define allocate_old_node() \
    (sxatcvar.atc_lv.u.Old.current_node == sxatcvar.atc_lv.u.Old.area_end \
     ? allocate_area () \
     : ( p_char_tmp = sxatcvar.atc_lv.u.Old.current_node , \
         p_char_tmp += sxatcvar.atc_lv.node_size         , \
        sxatcvar.atc_lv.u.Old.current_node = p_char_tmp  ))

#define allocate_node() \
    sxatcvar.atc_lv.early_visit_set == NULL ? allocate_old_node() : allocate_new_node()
    
#define free_node() \
    sxatcvar.atc_lv.early_visit_set == NULL ? freeareas() : free_tree()
    

#ifndef NBNODES
#define NBNODES 64
#endif

#ifdef SXRESIZE
  /* Increase area_size, to minimize number of allocations */
#define NEW_AREA_SIZE (sxatcvar.atc_lv.u.Old.area_size += NBNODES * sxatcvar.atc_lv.node_size)
#else
  /* Do not grow area_size, as it cannot be reallocated */
#define NEW_AREA_SIZE (sxatcvar.atc_lv.u.Old.area_size = NBNODES * sxatcvar.atc_lv.node_size)
#endif

SXNODE	***sxatc_stack (void)
{
    /* Si une specif par semat utilise des parsact, ces dernieres peuvent acceder aux sous-arbres
       deja construits :
       <terme>    = <variable> @i ;
       <variable> = %ID ; "ID"
       
       case i:
          (*sxatc_stack ()) [SXSTACKtop ()]->... permet d'acceder aux champs du noeud ID
    */
    return &stacktreenode;
}

static void sxatc_oflw (sxindex_header *index_hd, SXINT old_line_nb, SXINT old_size)
{
    SXINT i, j, k;

    sxuse (old_line_nb);
    sxuse (old_size);

    sxuse(index_hd); /* pour faire taire gcc -Wunused */
    k = sxatcvar.atc_lv.u.New.line_nb;
    sxatcvar.atc_lv.u.New.line_nb = i = sxindex_line_nb (sxatcvar.atc_lv.u.New.index_hd);
    sxatcvar.atc_lv.u.New.nodes = (SXNODE **) sxrealloc (sxatcvar.atc_lv.u.New.nodes, i, sizeof (SXNODE *));
    j = sxindex_column_nb (sxatcvar.atc_lv.u.New.index_hd);

    while (--i >= k)
	sxatcvar.atc_lv.u.New.nodes [i] = (SXNODE *) sxcalloc ((SXUINT)j, (SXUINT)sxatcvar.atc_lv.node_size);
}

static SXNODE	*allocate_area (void)
{
    struct sxatc_area		*old, *new;

    old = sxatcvar.atc_lv.u.Old.area;
    sxatcvar.atc_lv.u.Old.area = new = (struct sxatc_area*) sxcalloc (1, sizeof (struct sxatc_area) - sizeof (SXNODE) + NEW_AREA_SIZE);
    new->next = old;
    sxatcvar.atc_lv.u.Old.area_end = (char*) new->zone + (sxatcvar.atc_lv.u.Old.area_size - sxatcvar.atc_lv.node_size) /
	 sizeof (char);
    sxatcvar.atc_lv.u.Old.current_node = new->zone;
    return new->zone;
}



static void nodes_alloc (void)
{
    SXINT i, j;
    bool first_time = true;

    if (sxindex_is_allocated (sxatcvar.atc_lv.u.New.index_hd)) {
      first_time = false;
      sxindex_clear (sxatcvar.atc_lv.u.New.index_hd);
    }
    else {
      sxindex_alloc (&sxatcvar.atc_lv.u.New.index_hd, 2, 511, sxatc_oflw);
      sxprepare_for_possible_reset (sxatcvar.atc_lv.u.New.index_hd.set);
    }

    sxatcvar.atc_lv.u.New.line_nb = i = sxindex_line_nb (sxatcvar.atc_lv.u.New.index_hd);

    if (first_time)
      sxatcvar.atc_lv.u.New.nodes = (SXNODE **) sxalloc (i, sizeof (SXNODE *));

    j = sxindex_column_nb (sxatcvar.atc_lv.u.New.index_hd);

    if (first_time)
      while (--i >= 0)
	memset (sxatcvar.atc_lv.u.New.nodes [i], 0, /* (size_t) (*/ (SXUINT)j * (SXUINT)sxatcvar.atc_lv.node_size);
    else
      while (--i >= 0)
	sxatcvar.atc_lv.u.New.nodes [i] = (SXNODE *) sxcalloc ((SXUINT)j, (SXUINT)sxatcvar.atc_lv.node_size);
}

static void	freeareas (void)
{
    struct sxatc_area		*p, *q;

    q = sxatcvar.atc_lv.u.Old.area;

    while ((p = q) != NULL) {
	q = p->next;
	p->next = NULL;
	sxfree (p);
    }
    sxatcvar.atc_lv.u.Old.area_end = sxatcvar.atc_lv.u.Old.current_node = NULL;
    sxatcvar.atc_lv.u.Old.area = NULL;
    sxatcvar.atc_lv.abstract_tree_root = NULL;
}



static void free_tree (void)
{
    SXINT i = sxatcvar.atc_lv.u.New.line_nb;

    while (--i >= 0)
	sxfree (sxatcvar.atc_lv.u.New.nodes [i]);
    sxatcvar.atc_lv.abstract_tree_root = NULL;
}

static SXNODE *allocate_new_node (void)
{
    SXINT		x;
    SXNODE	*p;

    x = sxindex_seek (&(sxatcvar.atc_lv.u.New.index_hd));
    p = (SXNODE *)
       (sxatcvar.atc_lv.u.New.nodes [sxindex_i (sxatcvar.atc_lv.u.New.index_hd, x)] +
        sxindex_j (sxatcvar.atc_lv.u.New.index_hd, x) );
    p->index = x;
    return p;
}

void sub_tree_erase (SXNODE *node)
{
    SXNODE	*brother = node->son, *son;
    SXINT		sub_tree_erase_index;

    while ((son = brother) != NULL) {
	sub_tree_erase (son);
	sub_tree_erase_index = son->index;
	brother = son->brother;
	memset ((char*) son, 0, (size_t) sxatcvar.atc_lv.node_size); /* RAZ node */
	sxindex_release (sxatcvar.atc_lv.u.New.index_hd, sub_tree_erase_index);
    }
}


void sxatc_sub_tree_erase (/* SXNODE *node */ struct sxnode_header_s *node)
{
    /* Rend les noeuds du sous-arbre node (pas la racine) au systeme de gestion
       pour une reutilisation ulterieure eventuelle. */

    if (node != NULL) {
	sub_tree_erase (node);
	node->son = NULL;
	node->degree = 0;
    }
}


static SXINT	reccall = 0 /* compte les appels recursifs  */ ;

/* Variables communes a plusieurs procedures: */

static SXNODE	*brother, *son;
static SXINT	top, new_top;



static SXNODE	*rightmost_son (SXNODE *node_ptr)
{
    SXNODE	*next;

    while ((next = node_ptr->son) != NULL) {
	while ((next = (node_ptr = next)->brother) != NULL) {
	    /* void */
	}
    }

    return node_ptr;
}


/* 05/12/08 nouvelle version */
static void	cons_comments (bool is_right_cons, SXNODE *node_ptr, SXINT low, SXINT high)
{
    char	*com, *com1, *prev_com;
    SXINT	x, lgth, prev_com_lgth;

    prev_com_lgth = lgth = ((prev_com = node_ptr->token.comment) == NULL) ? 0 : strlen (prev_com);

    for (x = low; x <= high; x++) {
	if (SXSTACKtoken (x).lahead >= 0 /* not an action */  && (com1 = SXSTACKtoken (x).comment) != NULL)
	  lgth += strlen (com1);
    }

    if (lgth) {
      /* On reserve ds le comment_mngr la place juste necessaire pour y mettre ces commentaires */
      node_ptr->token.comment = com = sxcomment_book (&(sxplocals.sxcomment), lgth);

      if (is_right_cons && prev_com_lgth)
	com = sxcomment_catenate (com, prev_com, prev_com_lgth);

      for (x = low; x <= high; x++) {
	if (SXSTACKtoken (x).lahead >= 0 /* not an action */  && (com1 = SXSTACKtoken (x).comment) != NULL) {
	  lgth = strlen (com1);

	  if (lgth)
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wstringop-overflow"
#pragma GCC diagnostic ignored "-Wstringop-truncation"
#endif
	    com = sxcomment_catenate (com, com1, lgth);
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
#pragma GCC diagnostic pop
#endif
	}
      }

      if (!is_right_cons && prev_com_lgth)
	com = sxcomment_catenate (com, prev_com, prev_com_lgth);
    }	
}


static void	initialize (void)
     /* struct sxtables	*tables; */
/* On traite un nouveau texte source */
{
    sxatcvar.atc_lv.abstract_tree_root = NULL;
    sxatcvar.atc_lv.abstract_tree_is_error_node = false;

    if (reccall++ == 0)
	stacktreenode = (SXNODE**) sxalloc ((stns = stns_incr) + 1, sizeof (SXNODE*));

    if (sxatcvar.atc_lv.early_visit_set == NULL) {
	sxatcvar.atc_lv.u.Old.area = NULL;
	sxatcvar.atc_lv.u.Old.current_node = sxatcvar.atc_lv.u.Old.area_end = NULL;
    }
    else
	nodes_alloc ();
}



static void	finalize (void)
     /* struct sxtables	*tables; */
/* on a fini de traiter un texte source */
{
    if (!sxatcvar.atc_lv.are_comments_erased) {
	/* traitement du commentaire associe a EOF */

	if (sxsvar.sxlv.terminal_token.comment != NULL) {
	    SXINT       lgth, lgth1, lgth2;
	    SXNODE	*p;
	    char        *cat_com;

	    p = rightmost_son (sxatcvar.atc_lv.abstract_tree_root);

	    if (p->token.comment == NULL)
	      p->token.comment = sxsvar.sxlv.terminal_token.comment;
	    else {
	      if (sxsvar.sxlv.terminal_token.comment) {
		lgth1 = strlen (p->token.comment);
		lgth2 = strlen (sxsvar.sxlv.terminal_token.comment);
		lgth = lgth1+lgth2;
		cat_com = sxcomment_book (&(sxplocals.sxcomment), lgth);
		p->token.comment = sxcomment_catenate (sxcomment_catenate (cat_com, p->token.comment, lgth1), sxsvar.sxlv.terminal_token.comment, lgth2);
	      }
	    }
	}
    }

    if (sxatcvar.atc_lv.abstract_tree_root != NULL)
	sxatcvar.atc_lv.abstract_tree_root->father = NULL /* in case the root is a list */ ;

#ifdef SXRESIZE
    if (sxatcvar.atc_lv.early_visit_set == NULL && sxatcvar.atc_lv.u.Old.area != NULL) {
	sxatcvar.atc_lv.u.Old.area_end = sxatcvar.atc_lv.u.Old.current_node;
	sxatcvar.atc_lv.u.Old.area_size = ((SXNODE*)sxatcvar.atc_lv.u.Old.area_end - (SXNODE*) sxatcvar.atc_lv.u.Old.area->zone)
	  * sizeof (SXNODE) + sxatcvar.atc_lv.node_size;
	SXRESIZE (sxatcvar.atc_lv.u.Old.area, 1, sizeof (struct sxatc_area) - sizeof (SXNODE) + sxatcvar.atc_lv.u.Old.area_size);
    }
#endif

    if (--reccall == 0)
      sxfree (stacktreenode), stacktreenode = NULL;
}



static void	test_and_create_son (SXINT x)
{
/* x reference soit un nt soit un t generique */
    struct sxtoken	*token = &(SXSTACKtoken (x));

    if (token->lahead > 0) {
	/* t generique */
	brother = allocate_node ();
	brother->token = *token;
	brother->name = sxatcvar.SXT_tables.T_ter_to_node_name [token->lahead];
    }
    else
	brother = stacktreenode [x];

    brother->father = sxatcvar.atc_lv.abstract_tree_root;
}






static void	c_family (void)
{
    SXINT	i, bsup, pos;
    SXINT	binf, low;

    bsup = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce () + 1].T_ss_indx;
    binf = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_ss_indx;
    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    sxatcvar.atc_lv.abstract_tree_root->name = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_node_name;
    pos = 0;

    if (bsup > binf) {
	test_and_create_son (top - sxatcvar.SXT_tables.T_stack_schema [bsup - 1]);
	brother->position = ++pos;
	sxatcvar.atc_lv.abstract_tree_root->son = brother;

	for (i = bsup - 2; i >= binf; i--) {
	    son = brother;
	    test_and_create_son (top - sxatcvar.SXT_tables.T_stack_schema [i]);
	    brother->position = ++pos;
	    son->brother = brother;
	}
    }
    else
	/* La RHS ne contient que des terminaux non-generiques (ou est vide), on met
	   le look-ahead du plus a gauche */
	sxatcvar.atc_lv.abstract_tree_root->token.lahead = SXSTACKtoken (new_top).lahead;

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	low = new_top;

	if (bsup > binf) {
	    bool	is_first = true;
	    SXNODE	*node_ptr = sxatcvar.atc_lv.abstract_tree_root;

	    for (i = bsup - 1; i >= binf; i--) {
		SXINT	j;

		if (is_first) {
		    is_first = false;
		    node_ptr = node_ptr->son;
		}
		else
		    node_ptr = node_ptr->brother;

		if ((j = top - sxatcvar.SXT_tables.T_stack_schema [i]) > low)
		  cons_comments (false, node_ptr, low, j - 1);

		low = j + 1;
	    }

	    if (top >= low)
		cons_comments (true, rightmost_son (node_ptr), low, top);
	}
	else if (top >= low)
	    cons_comments (false, sxatcvar.atc_lv.abstract_tree_root, low, top);
    }

    sxatcvar.atc_lv.abstract_tree_root->token.source_index = SXSTACKtoken (new_top).source_index;
}



static void	add_right_list_node (void)
{
    SXINT	i, low, pos, high;
    SXNODE	*node_ptr;

    i = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_ss_indx;
    high = top - sxatcvar.SXT_tables.T_stack_schema [i];
    low = top - sxatcvar.SXT_tables.T_stack_schema [i + 1];
    sxatcvar.atc_lv.abstract_tree_root = stacktreenode [high];
    son = sxatcvar.atc_lv.abstract_tree_root->son;
    test_and_create_son (low);
    node_ptr = sxatcvar.atc_lv.abstract_tree_root->son = brother;
    node_ptr->brother = son;
    node_ptr->first_list_element = true;

    if (son != NULL)
	son->first_list_element = false;
    else
	node_ptr->last_list_element = true;

    pos = 0;

    while (brother != NULL) {
	brother->position = ++pos;
	brother = brother->brother;
    }

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (low > new_top)
	    cons_comments (false, node_ptr, new_top, low - 1);

	if (--high > low)
	    cons_comments (true, rightmost_son (node_ptr), low + 1, high);
    }

    sxatcvar.atc_lv.abstract_tree_root->token.source_index = SXSTACKtoken (new_top).source_index;
}



static void	add_list_node (void)
{
    SXINT	i, low, high;

    i = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_ss_indx;
    high = top - sxatcvar.SXT_tables.T_stack_schema [i];
    low = top - sxatcvar.SXT_tables.T_stack_schema [i + 1];
    sxatcvar.atc_lv.abstract_tree_root = stacktreenode [low];
    son = sxatcvar.atc_lv.abstract_tree_root->father;
    /* the most RHS son */

    test_and_create_son (high);

    if (son == NULL) {
	sxatcvar.atc_lv.abstract_tree_root->son = brother;
	brother->first_list_element = true;
    }
    else {
	son->brother = brother;
	son->last_list_element = false;
    }

    brother->position = ++sxatcvar.atc_lv.abstract_tree_root->degree;
    brother->last_list_element = true;
    sxatcvar.atc_lv.abstract_tree_root->father = brother;


/* new most RHS son */

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (--high >= ++low)
	    cons_comments (false, brother, low, high);

	if (--high >= ++low)
	    cons_comments (true, rightmost_son (brother), low, top);
    }
}



static void	create_error_family (void)
{
    SXINT	j, pos, low, lahead;
    struct sxtoken	*tok;
    SXNODE	*node_ptr;

    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    sxatcvar.atc_lv.abstract_tree_root->name = ERROR_n;
    sxatcvar.atc_lv.abstract_tree_root->is_list = true;
    pos = 0;
    low = new_top;

    for (j = new_top; j <= top; j++) {
	/* j = parse_stack index */
	lahead = (tok = &(SXSTACKtoken (j)))->lahead;

	if (lahead == 0) /* non terminal */
	     node_ptr = stacktreenode [j];
	else if (lahead > 0) /* terminal */ {
	    if (sxgenericp (sxplocals.sxtables, lahead)) {
		/* "generic" terminal */
		node_ptr = allocate_node ();
		node_ptr->token = *tok;
		node_ptr->name = sxatcvar.SXT_tables.T_ter_to_node_name [lahead];
	    }
	    else node_ptr = NULL;
	}
	else node_ptr = NULL;

	if (node_ptr != NULL) {
	    node_ptr->father = sxatcvar.atc_lv.abstract_tree_root;
	    node_ptr->position = ++pos;

	    if (pos == 1) {
		sxatcvar.atc_lv.abstract_tree_root->son = node_ptr;
		node_ptr->first_list_element = true;
	    }
	    else
		son->brother = node_ptr;

	    son = node_ptr;

	    if (!sxatcvar.atc_lv.are_comments_erased) {
		if (j > low)
		    cons_comments (false, node_ptr, low, j - 1);

		low = j + 1;
	    }
	}
    }

    if (pos != 0) {
	son->last_list_element = true;
	sxatcvar.atc_lv.abstract_tree_root->father = son /* ptr to the most RHS son */ ;

	if (!sxatcvar.atc_lv.are_comments_erased) {
	    if (top >= low)
		cons_comments (true, son, low, top);
	}
    }
    else {
	if (!sxatcvar.atc_lv.are_comments_erased && top >= low)
	    cons_comments (false, sxatcvar.atc_lv.abstract_tree_root, low, top);
    }

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;
    sxatcvar.atc_lv.abstract_tree_root->token.source_index = SXSTACKtoken (new_top).source_index;
}



static void	c_leaf (void)
{
    SXINT	x;

    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    x = top - sxatcvar.SXT_tables.T_stack_schema [sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_ss_indx];
    sxatcvar.atc_lv.abstract_tree_root->token = SXSTACKtoken (x);
    sxatcvar.atc_lv.abstract_tree_root->name = sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_node_name;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (x > new_top)
	    cons_comments (false, sxatcvar.atc_lv.abstract_tree_root, new_top, x - 1);

	if (top > x)
	    cons_comments (true, sxatcvar.atc_lv.abstract_tree_root, x + 1, top);
    }
}



static void	atc_open (struct sxtables *tables)
                   	        
/* On traite un nouveau langage */
{
    sxatcvar.SXT_tables = tables->sem_tables->SXT_tables;
    sxatcvar.atc_lv.node_size = sizeof (SXNODE) /* default value for dummy smp */ ;
    sxatcvar.atc_lv.are_comments_erased = tables->SXS_tables.S_are_comments_erased;
    sxatcvar.atc_lv.early_visit_set = NULL;
    (*sxatcvar.SXT_tables.T_sempass) (SXOPEN, tables);

    if (sxatcvar.atc_lv.early_visit_set == NULL)
	sxatcvar.atc_lv.u.Old.area_size = 0;
}



static void	action (SXINT act)
{
    SXINT	j;

    top = SXSTACKtop ();
    new_top = SXSTACKnewtop ();

    /* Dans tous les cas (meme production vide ou action), "SXSTACKtoken (new_top).source_index"
       est positionne' correctement par le parser. */

    if (top >= stns) {
	stacktreenode = (SXNODE**) sxrealloc (stacktreenode, (stns = top + stns_incr) + 1, sizeof (SXNODE*));
    }

    switch (act) {
    case 0:
	j = top - sxatcvar.SXT_tables.T_stack_schema [sxatcvar.SXT_tables.SXT_node_info [SXSTACKreduce ()].T_ss_indx];
	sxatcvar.atc_lv.abstract_tree_root = stacktreenode [j];

	if (!sxatcvar.atc_lv.are_comments_erased) {
	    if (j > new_top)
		cons_comments (false, sxatcvar.atc_lv.abstract_tree_root, new_top, j - 1);

	    if (top > j)
		cons_comments (true, rightmost_son (sxatcvar.atc_lv.abstract_tree_root), j + 1, top);
	}

	if (j > new_top)
	    /* Il y a des terminaux non-generiques plus a gauche */
	    sxatcvar.atc_lv.abstract_tree_root->token.source_index = SXSTACKtoken (new_top).source_index;

	break;

    case 1:
	c_family ();
	break;

    case 2:
	add_list_node ();
	break;

    case 3:
	add_right_list_node ();
	break;

    case 4:
	create_error_family ();
	break;

    case 5:
	c_leaf ();
	break;

    case 6:
	c_family ();
	sxatcvar.atc_lv.abstract_tree_root->is_list = true;

	if ((son = sxatcvar.atc_lv.abstract_tree_root->son) != NULL) {
	    son->first_list_element = true;
	    brother->last_list_element = true;
	    sxatcvar.atc_lv.abstract_tree_root->father = brother; /* new most RHS son */
	}

	break;

    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap("sxatc","unknown switch case #1");
#endif
      break;
    }

    stacktreenode [new_top] = sxatcvar.atc_lv.abstract_tree_root;
}



void sxatc (SXINT what, SXINT action_no, struct sxtables *arg)
    /* si what=SXACTION, action_no est un entier entre 0 et 6 ;  si what != SXACTION, arg est un pointeur vers des tables. */
{
    SXINT l;

    switch (what) {
    case SXOPEN:
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "atc");
	atc_open (arg);
	break;

    case SXINIT:
      initialize (/* arg */);
	break;

    case SXFINAL:
      finalize (/* arg */);
	break;

    case SXACTION:
      /* puisque what=SXACTION, arg n'est pas un pointeur vers des tables, mais bien un entier entre 0
	 et 6, on veut que le compilo C ne hurle pas sur un probleme de typage, d'ou le(s?) cast(s?) */
	action (action_no);

	/* pbs sur les listes..., appel sur chaque nouveau fils. */
	if (sxatcvar.atc_lv.early_visit_set != NULL &&
	    SXBA_bit_is_set (sxatcvar.atc_lv.early_visit_set, sxatcvar.atc_lv.abstract_tree_root->name))
	    (*sxatcvar.SXT_tables.T_sempass) (SXSEMPASS, arg);

	break;

    case SXERROR:
	sxatcvar.atc_lv.abstract_tree_is_error_node = true;

	if ((l = action_no) > 0) {
	    action ((SXINT) 4);

	    while (--l > 0) {
		if (++new_top >= stns)
		    stacktreenode = (SXNODE**)
			sxrealloc (stacktreenode, (stns = new_top + stns_incr) + 1, sizeof (SXNODE*));

		sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
		sxatcvar.atc_lv.abstract_tree_root->name = ERROR_n;
		sxatcvar.atc_lv.abstract_tree_root->is_list = true;
		stacktreenode [new_top] = sxatcvar.atc_lv.abstract_tree_root;
	    }
	}

	break;

    case SXSEMPASS:
	(*sxatcvar.SXT_tables.T_sempass) (SXACTION, arg);
	break;

    case SXCLOSE:
	free_node ();
	(*sxatcvar.SXT_tables.T_sempass) (SXCLOSE, arg);
	break;

    default:
	fprintf (sxstderr, "The function \"sxatc\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}
