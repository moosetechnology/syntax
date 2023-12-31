/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1984 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */



/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 10:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* 25-04-88 09:05 (pb) :	Modification a l'acces au look_ahead	*/
/************************************************************************/
/* 03-12-87 12:24 (phd&pb) :	Meme chose, mais en plus fin		*/
/************************************************************************/
/* 02-12-87 16:42 (phd) :	On ne construit plus de noeud inutile	*/
/*				sur une reduction simple		*/
/************************************************************************/
/* 30-11-87 14:05 (phd) :	Suppression de "post_source_index"	*/
/************************************************************************/
/* 26-11-87 11:02 (phd) :	On n'alloue des zones de taille		*/
/*				croissante que si RESIZE est definie	*/
/************************************************************************/
/* 20-11-87 14:12 (phd) :	Utilisation de la macro SXRESIZE	*/
/************************************************************************/
/* 19-11-87 10:35 (phd) :	Renommage node_pp => sxnode_pp		*/
/************************************************************************/
/* 18-11-87 16:20 (phd) :	Ajout et utilisation de NBNODES		*/
/************************************************************************/
/* 18-11-87 12:07 (phd) :	Allocations de tailles croissantes et	*/
/*				correction d'une bogue monstrueuse	*/
/************************************************************************/
/* 15-06-87 10:00 (pb) :	Adaptation aux actions et predicats	*/
/*				+ ameliorations				*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 02-03-87 08:53 (pb) :	Utilisation de atoks pour <LHS> -> vide	*/
/************************************************************************/
/* 11-02-87 13:43 (pb) :	Simplification <LHS> -> vide		*/
/************************************************************************/
/* 01-12-86 11:12 (pb&phd) :	Appel de sxcheck_magic_number		*/
/************************************************************************/
/* 17-10-86 13:53 (phd):	Modification du nom du paragrapheur	*/
/************************************************************************/
/* 16-10-86 15:48 (phd):	Correction de string_cat et cons_com	*/
/************************************************************************/
/* 16-10-86 14:41 (phd):	Appels au paragrapheur			*/
/************************************************************************/
/* 16-10-86 14:40 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


/*************************************************************/
/*                                                           */
/*  This program has been translated from l_atc_pp.pl1       */
/*  on Monday the seventeenth of March, 1986, at 15:58:46,   */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/










/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */
/*											*/
/*											*/
/* Historique des modifications en ordre chronologique inverse:				*/
/* -----------------------------------------------------------				*/
/*											*/
/* 5 Fevrier 1986:									*/
/* Appel de "le_paragrapheur".  Par PB.                                         	*/
/*											*/
/* 9 Decembre 1985:									*/
/* Linearisation de l'arbre.  Par PB et PhD.						*/
/*											*/
/* 18 Avril 1985:									*/
/* Appel des points d'entree open, terminate, sweep, close, cleanup de paragrapheur	*/
/*											*/
/* 14 Fevrier 1985:									*/
/* Mise a niveau des points d'entree							*/
/*											*/
/* 20 Mars 1984:									*/
/* Suppression du champ "father" dans les noeuds.  Il est remplace par un "brother" non	*/
/* null dans le dernier fils.  Par PB.							*/
/*											*/
/* 19 Decembre 1983:									*/
/* Allocations dans un grand tableau et non plus dans une area, par PB et PhD.		*/
/*											*/
/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  */

#include "sxunix.h"

char WHAT_SXATCPP[] = "@(#)SYNTAX - $Id: sxatcpp.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

/* Gestion dans le tas : */

static int	stack_size;
struct tree_node_stack {
    struct sxnode_pp	*head, *tail;
};
static struct tree_node_stack	*tree_node_stack /* 1:stack_size */ ;

#define allocate_node() (node == area_end ? allocate_area () : ++node)

#ifndef NBNODES
# define NBNODES 128
#endif

#ifdef SXRESIZE
  /* Increase area_size, to minimize number of allocations */
# define NEW_AREA_SIZE (area_size += NBNODES)
#else
  /* Do not grow area_size, as it cannot be reallocated */
# define NEW_AREA_SIZE (area_size = NBNODES)
#endif

struct sxatcpp_area {
    struct sxatcpp_area	*next;
    struct sxnode_pp	zone [1];
};
static int	area_size;
static struct sxatcpp_area	*area;
static struct sxnode_pp	*area_end, *node;



static struct sxnode_pp	*allocate_area ()
{
    register struct sxatcpp_area	*old, *new;

    old = area;
    area = new = (struct sxatcpp_area*) sxcalloc (1, sizeof (struct sxatcpp_area) + NEW_AREA_SIZE * sizeof (struct sxnode_pp));
    new->next = old;
    area_end = (node = new->zone) + area_size;
    return node;
}



static VOID	free_areas ()
{
    register struct sxatcpp_area	*prev, *next;
    next = area;

    while ((prev = next) != NULL) {
	next = next->next;
	sxfree (prev);
    }
}



static struct sxnode_pp	*father_ptr, *prv_ptr;
static char	*com_ref;




#define cons_com(c1,c2) (c1==NULL?c2:(c2==NULL?c1:string_cat(c1,c2)))

static char	*string_cat (s1, s2)
    register char	*s1, *s2;
{
    register char	*t1, *t2;
    register int	l1;

    s1 = (char*) sxrealloc (s1, (l1 = strlen (s1)) + strlen (s2) + 1, sizeof (char));
    t1 = s1 + l1, t2 = s2;

    while ((*t1++ = *t2++) != NUL) {
	/* null; */
    }

    sxfree (s2);
    return s1;
}



static VOID	initialize ()
{
    stack_size = 128;
    area_size = 0;
    area = NULL;
    node = area_end = NULL;
    father_ptr = NULL;
    tree_node_stack = (struct tree_node_stack*) sxalloc (stack_size + 1, sizeof (struct tree_node_stack));
}



static VOID	finalize ()
{
/* le commentaire eventuel se trouvant en fin de  */
/* programme devient le post_comment de la racine */
    father_ptr->post_comment = cons_com (father_ptr->post_comment, sxsvar.lv.terminal_token.comment);

#ifdef SXRESIZE
    if (area != NULL) {
	SXRESIZE (area, 1, sizeof (struct sxatcpp_area) + (area_size = (area_end = node) - area->zone) * sizeof (struct sxnode_pp));
    }
#endif

    sxfree (tree_node_stack);
}



static VOID	action ()
{
    register int	top, new_top, xs;
    register struct sxtoken	*token;
    register struct sxnode_pp	*brother, *son, *father;
    register BOOLEAN	is_first = TRUE;

    top = STACKtop ();
    new_top = STACKnewtop ();

    if (top >= stack_size)
	tree_node_stack = (struct tree_node_stack*) sxrealloc (tree_node_stack, (stack_size *= 2) + 1, sizeof (struct
	     tree_node_stack));

    son = father = allocate_node ();
    father->name = STACKreduce ();

/* Initialisation: on met a priori le source_index du look_ahead */

    father->terminal_token.source_index = SXGET_TOKEN (sxplocals.atok_no).source_index;
    com_ref = NULL;
    prv_ptr = NULL;

    for (xs = new_top; xs <= top; xs++) {
	token = &(STACKtoken (xs));

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
	    is_first = FALSE;
	}
    }

    father->post_comment = com_ref;
    tree_node_stack [new_top].head = father_ptr = father;
    tree_node_stack [new_top].tail = son;
}



static VOID	error ()
{
    father_ptr = allocate_node ();
    father_ptr->name = 0 /* error node */ ;
    tree_node_stack [STACKnewtop ()].head = tree_node_stack [STACKnewtop ()].tail = father_ptr;
}



VOID	sxatcpp (sxatcpp_what, arg)
    int		sxatcpp_what;
    struct sxtables	*arg;	/* or action number */
{
    switch (sxatcpp_what) {
    case OPEN:
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "atcpp");

    case CLOSE:
	sxppp (sxatcpp_what, arg);
	break;

    case INIT:
	initialize ();
	break;

    case FINAL:
	finalize ();
	break;

    case ACTION:
	if ((long) arg != 0) {	/* 0 when simple reduction */
	    action ();
	}
	break;

    case SEMPASS:
	if (sxverbosep) {
	    fputs ("Pretty Printer ", sxtty);
	}

	/* dans le cas ou sxppp() est appelee avec un premier parametre valant
        * ACTION, le deuxieme parametre, formellement declare avec le type
        * "(struct sxtables *)", doit etre de type "(struct sxnode_pp *)" en
        * realite : l'interieur de la fonction sxppp() contient un cast dans
        * ce cas */
	sxppp (ACTION, (struct sxtables *) father_ptr);
	free_areas ();
	break;

    case ERROR:
	error ();
	break;

    default:
	fprintf (sxstderr, "The function \"sxatcpp\" is out of date with respect to its specification.\n");
	abort ();
    }
}
