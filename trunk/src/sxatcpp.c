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

static char	ME [] = "ATCPP";

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXATCPP[] = "@(#)SYNTAX - $Id: sxatcpp.c 4166 2024-08-19 09:00:49Z garavel $" WHAT_DEBUG;

/* Gestion dans le tas : */

static SXINT	stack_size;
struct tree_node_stack {
    struct sxnode_pp	*head, *tail;
};
static struct tree_node_stack	*tree_node_stack /* 1:stack_size */ ;

#define allocate_node() (node == area_end ? allocate_area () : ++node)

#ifndef NBNODES
#define NBNODES 128
#endif

#ifdef SXRESIZE
  /* Increase area_size, to minimize number of allocations */
#define NEW_AREA_SIZE (area_size += NBNODES)
#else
  /* Do not grow area_size, as it cannot be reallocated */
#define NEW_AREA_SIZE (area_size = NBNODES)
#endif

struct sxatcpp_area {
    struct sxatcpp_area	*next;
    struct sxnode_pp	zone [1];
};
static SXINT	area_size;
static struct sxatcpp_area	*area;
static struct sxnode_pp	*area_end, *node;



static struct sxnode_pp	*allocate_area (void)
{
    struct sxatcpp_area	*old, *new;

    old = area;
    area = new = (struct sxatcpp_area*) sxcalloc (1, sizeof (struct sxatcpp_area) + NEW_AREA_SIZE * sizeof (struct sxnode_pp));
    new->next = old;
    area_end = (node = new->zone) + area_size;
    return node;
}



static void	free_areas (void)
{
    struct sxatcpp_area	*prev, *next;
    next = area;

    while ((prev = next) != NULL) {
	next = next->next;
	sxfree (prev);
    }
}



static struct sxnode_pp	*root_ptr, *prv_ptr;
static char	*com_ref;

#define cons_com(c1,c2) ((c1)==NULL?(c2):((c2)==NULL?(c1):string_cat((c1),(c2))))

static char *string_cat (char *s1, char *s2)
{
    char	*t1, *t2;
    size_t	l1;

    s1 = (char*) sxrealloc (s1, (l1 = strlen (s1)) + strlen (s2) + 1, sizeof (char));
    t1 = s1 + l1, t2 = s2;

    while ((*t1++ = *t2++) != SXNUL) {
	/* null; */
    }

    sxfree (s2);
    return s1;
}



static void	initialize (void)
{
    stack_size = 128;
    area_size = 0;
    area = NULL;
    node = area_end = NULL;
    root_ptr = NULL;
    tree_node_stack = (struct tree_node_stack*) sxalloc (stack_size + 1, sizeof (struct tree_node_stack));
}

#ifdef BEFORE_PPTREE_WALK
  /* Avant l'exploitation de l'arbre que l'on vient de construire, on donne la possibilité à l'utilisateur
     de reprendre la main pour le compléter.  C'est par exemple utilisé dans ppf77 pour insérer directement
     les commentaires dans l'arbre (plutôt que par le mécanisme usuel : token -> parse_item -> noeud) en
     utilisant l'option de compilation de sxatcpp.c -DBEFORE_PPTREE_WALK=f77_process_postponed_comments. */
  extern void BEFORE_PPTREE_WALK (struct sxnode_pp *);
#endif

static void	finalize_action (void)
{
  sxuse (ME); /* Pour compilos susceptibles!! */

#ifdef BEFORE_PPTREE_WALK
  BEFORE_PPTREE_WALK (root_ptr);
#endif

#ifdef SXRESIZE
  if (area != NULL) {
    SXRESIZE (area, 1, sizeof (struct sxatcpp_area) + (area_size = (area_end = node) - area->zone) * sizeof (struct sxnode_pp));
  }
#endif
}


static void	finalize (void)
{
  sxfree (tree_node_stack);
  tree_node_stack = NULL;
}


#if BUG
static void print_tok (struct sxtoken	*pt)
{
  fprintf (stdout,
	   "%s\t\ttoken = (%ld, \"%s\")\n\
\t\t\tste = (%ld, \"%s\")\n\
\t\t\tsource_index = (%s, %ld, %ld)\n\
\t\t\tcomment = \"%s\"\n\
",
	  ME,
	  pt->lahead,
	  sxttext (sxsvar.sxtables, pt->lahead),
	  pt->string_table_entry,
	  pt->string_table_entry > 1 ? sxstrget (pt->string_table_entry) : 
	  (pt->string_table_entry == 1 ? "SXEMPTY_STE" : "SXERROR_STE"),
	  pt->source_index.file_name,
	  (SXUINT) pt->source_index.line,
	  (SXUINT) pt->source_index.column,
	  (pt->comment != NULL) ? pt->comment : "(NULL)"
	  );
}

static void	print_atcppnode (struct sxnode_pp *ppnode, SXINT xps)
{
    if (ppnode != NULL) {
        fprintf (stdout, "%s : cur_root_node = %lX (xps = %ld):\tnext = %lX\n\t",
		ME, (SXUINT) ppnode, xps, (SXUINT) ppnode->next);

	if (ppnode->terminal_token.comment != NULL) {
	    fprintf (stdout, "comment = \"%s\"\n\t", ppnode->terminal_token.comment);
	}

	if (ppnode->name > 0) {
	    fprintf (stdout, "Prod = %ld", ppnode->name);
	}
	else if (ppnode->name == 0) {
	  fputs ("ERROR SXNODE", stdout);
	}
	else {
	    fprintf (stdout, "ste = %s (%s)",
		     sxstrget (ppnode->terminal_token.string_table_entry), sxttext (sxppvariables.sxtables, -ppnode->name));
	}

	if (ppnode->post_comment != NULL) {
	    fprintf (stdout, "\n\tpost_comment = \"%s\"", ppnode->post_comment);
	}
    }
    else
	fputs ("Node : NULL", stdout);
}
#endif

static void	action (void)
{
    SXINT	top, new_top, xs;
    struct sxtoken	*token;
    struct sxnode_pp	*brother, *son, *father;
    bool	is_first = true;

    top = SXSTACKtop ();
    new_top = SXSTACKnewtop ();

    if (top >= stack_size)
	tree_node_stack = (struct tree_node_stack*) sxrealloc (tree_node_stack, (stack_size *= 2) + 1, sizeof (struct
	     tree_node_stack));

    son = father = allocate_node ();
    father->name = SXSTACKreduce ();

/* Initialisation: on met a priori le source_index du look_ahead */

    father->terminal_token.source_index = SXGET_TOKEN (sxplocals.atok_no).source_index;
    com_ref = NULL;
    prv_ptr = NULL;

#if BUG
    fputs ("\n--------------------------------------------------------\n", stdout);
    fprintf (stdout, "%s\tEntering action()\n", ME);
#endif

    for (xs = new_top; xs <= top; xs++) {
	token = &(SXSTACKtoken (xs));

#if BUG
	print_tok (token);
#endif

	if (token->lahead >= 0 /* nt or t */ ) {
	    if (is_first)
		father->terminal_token.source_index = token->source_index;

	    if (token->lahead == 0) {
		/* non-terminal */
		brother = son->next = tree_node_stack [xs].head;
		son = tree_node_stack [xs].tail;
	    }
	    else if (sxgenericp (sxplocals.sxtables, token->lahead)) {
		son = son->next = brother = allocate_node ();
		brother->terminal_token = *token;
		brother->name = -token->lahead;
	    }
	    else
		brother = NULL;

	    if (brother != NULL) {
		brother->terminal_token.comment = cons_com (com_ref, brother->terminal_token.comment);
		com_ref = NULL;
	    }
	    else if (is_first)
		father->terminal_token.comment = token->comment;
	    else if (prv_ptr != NULL)
		prv_ptr->post_comment = cons_com (prv_ptr->post_comment, token->comment);
	    else
		com_ref = cons_com (com_ref, token->comment);

	    prv_ptr = brother;
	    is_first = false;
	}
    }

    father->post_comment = com_ref;

#if BUG
    print_atcppnode (father, new_top);
    fprintf (stdout, "\n%s\tLeaving action()\n", ME);
    fputs ("--------------------------------------------------------\n", stdout);
#endif

    tree_node_stack [new_top].head = root_ptr = father;
    tree_node_stack [new_top].tail = son;
}



static void	error (void)
{
    root_ptr = allocate_node ();
    root_ptr->name = 0 /* error node */ ;
    tree_node_stack [SXSTACKnewtop ()].head = tree_node_stack [SXSTACKnewtop ()].tail = root_ptr;
}



void	sxatcpp (SXINT what, SXINT action_no, SXTABLES *arg)
{
    switch (what) {
    case SXOPEN:
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "atcpp");
#if defined (__GNUC__) && (__GNUC__ - 0 >= 7)
	__attribute__ ((fallthrough));
#endif

    case SXCLOSE:
	sxppp (what, arg);
	break;

    case SXINIT:
	initialize ();
	break;

    case SXFINAL:
	finalize ();
	break;

    case SXACTION:
	if (action_no != 0) {	/* 0 when simple reduction */
	    action ();
	}
	break;

    case SXSEMPASS:
        if (sxerrmngr.nbmess [2] != 0)
	  break;
	    
	if (sxverbosep) {
	  fputs ("Pretty Printer ", sxtty);
	}

	finalize_action ();

	/* dans le cas ou sxppp() est appelee avec un premier parametre valant
	 * SXACTION, le deuxieme parametre, formellement declare avec le type
	 * "(SXTABLES *)", doit etre de type "(struct sxnode_pp *)" en
	 * realite : l'interieur de la fonction sxppp() contient un cast dans
	 * ce cas */
	sxppp (SXACTION, (SXTABLES *) root_ptr);
	free_areas ();
	break;

    case SXERROR:
	error ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxatcpp\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}
