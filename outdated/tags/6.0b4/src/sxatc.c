/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe Langages et Traducteurs.        *
   *                                                      *
   ******************************************************** */



/* Actions de construction d'arbre pour la nouvelle version de SYNTAX (TM) */

 
/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 10:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* Ven 14 Nov 1997(pb) :	Modif c_family si rhs \in non-generic*	*/
/*				token.lahead est positionne		*/
/************************************************************************/
/* 31-03-94 13:44 (pb) :	Marche arriere sur le traitement des	*/
/*				"source_index" des noeuds		*/
/************************************************************************/
/* 02-04-93 16:03 (pb) :	Test de abstract_tree_root!=NULL dans	*/
/*				finalize (au cas ou...)			*/
/************************************************************************/
/* 24-02-93 15:35 (pb) :	Le point d'entree "ERROR" a un 2eme	*/
/*				parametre qui donne le nombre d'etats	*/
/*				ajoutes par le rattrapage global (avant	*/
/*				1 seul).				*/
/************************************************************************/
/* 23-02-93 11:11 (pb) :	Le champ token d'une feuille decrivant	*/
/*				un mot non vide forme de terminaux non	*/
/*				generiques est le token du terminal le	*/
/*				plus a gauche.				*/
/************************************************************************/
/* 10-02-93 14:35 (pb) :	Bug ds ess_tok_mngr pour les comments	*/
/************************************************************************/
/* 21-01-93 11:25 (pb) :	Changement ds le traitement des source-	*/
/*				index des noeuds.			*/
/************************************************************************/
/* 07-01-93 14:40 (pb) :	Permet l'appel de la semantic_pass au	*/
/*				cours de la construction de l'arbre	*/
/*				L'utisateur peut detruire des 		*/
/*				sous-arbres par "sxatc_sub_tree_erase"	*/
/************************************************************************/
/* 12-08-91 15:20 (pb) :	"freeareas ()" est passe ds "CLOSE"	*/
/*				a la demande de GEC-ALSTHOM		*/
/************************************************************************/
/* 21-02-90 11:20 (pb) :	Bug dans cons_comments			*/
/************************************************************************/
/* 16-11-88 10:49 (pb) :	Ajout de la fonction externe sxatc_stack*/
/************************************************************************/
/* 26-09-88 17:35 (pb) :	Bug ds add_right_list_node (ds PL/1=>C)	*/
/************************************************************************/
/* 25-04-88 09:05 (pb) :	Modification a l'acces au look_ahead	*/
/************************************************************************/
/* 11-01-88 14:21 (phd&pb) :	Renommage de entry en what dans sxatc	*/
/************************************************************************/
/* 26-11-87 11:02 (phd) :	On n'alloue des zones de taille		*/
/*				croissante que si RESIZE est definie	*/
/************************************************************************/
/* 20-11-87 14:12 (phd) :	Utilisation de la macro SXRESIZE	*/
/************************************************************************/
/* 20-11-87 11:16 (pb&phd) :	Remise du pere de la racine a NULL.	*/
/*				Cette instruction avait disparu...	*/
/************************************************************************/
/* 19-11-87 17:13 (phd) :	Suppression de la variable "father"	*/
/************************************************************************/
/* 19-11-87 12:07 (phd) :	Allocations de tailles croissantes et	*/
/*				correction d'une bogue monstrueuse	*/
/************************************************************************/
/* 05-10-87 11:15 (pb) :	Bug dans create_error_family   		*/
/************************************************************************/
/* 24-06-87 10:31 (pb) :	Bug sur last_list_element   		*/
/************************************************************************/
/* 15-06-87 09:35 (pb) :	Adaptation aux actions et predicats	*/
/*				+ correction bug			*/
/*				+ ameliorations				*/
/************************************************************************/
/* 25-05-87 11:03 (phd) :	Ajout d'un message d'erreur		*/
/************************************************************************/
/* 31-03-87 13:25 (pb) :	Gestion du source_index sur les non	*/
/*				terminaux	        		*/
/************************************************************************/
/* 29-12-86 13:25 (pb) :	Sur OPEN node_size = sizeof (NODE) pour	*/
/*				dummy semantic_pass       		*/
/************************************************************************/
/* 16-12-86 17:25 (phd) :	Un grand coup de balai!!!   		*/
/************************************************************************/
/* 04-12-86 14:52 (pb) :	Un bug de plus en moins	   		*/
/************************************************************************/
/* 01-12-86 11:12 (pb&phd) :	Appel de sxcheck_magic_number		*/
/************************************************************************/
/* 21-10-86 11:22 (phd):	Suppression de la declaration d'une	*/
/*				"semantic_pass" externe			*/
/************************************************************************/
/* 21-10-86 11:22 (phd):	Ajout du "copyright"			*/
/************************************************************************/
/* 21-10-86 11:22 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#define SXATC_AREA	struct sxatc_area	/* used in "sxunix.h" */

#include "sxunix.h"
#include "sxba.h"

char WHAT_SXATC[] = "@(#)SYNTAX - $Id: sxatc.c 1047 2008-02-15 08:41:15Z rlacroix $" WHAT_DEBUG;

struct sxatc_area {
    struct sxatc_area	*next;
    SXNODE		zone [1];
};

#define ERROR_n 1

/* Gestion dans le tas : */

static NODE	**stacktreenode /* [stns] */ ;
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
# define NBNODES 64
#endif

#ifdef SXRESIZE
  /* Increase area_size, to minimize number of allocations */
# define NEW_AREA_SIZE (sxatcvar.atc_lv.u.Old.area_size += NBNODES * sxatcvar.atc_lv.node_size)
#else
  /* Do not grow area_size, as it cannot be reallocated */
# define NEW_AREA_SIZE (sxatcvar.atc_lv.u.Old.area_size = NBNODES * sxatcvar.atc_lv.node_size)
#endif

NODE	***sxatc_stack (void)
{
    /* Si une specif par semat utilise des parsact, ces dernieres peuvent acceder aux sous-arbres
       deja construits :
       <terme>    = <variable> @i ;
       <variable> = %ID ; "ID"
       
       case i:
          (*sxatc_stack ()) [STACKtop ()]->... permet d'acceder aux champs du noeud ID
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
    sxatcvar.atc_lv.u.New.nodes = (NODE **) sxrealloc (sxatcvar.atc_lv.u.New.nodes, i, sizeof (NODE *));
    j = sxindex_column_nb (sxatcvar.atc_lv.u.New.index_hd);

    while (--i >= k)
	sxatcvar.atc_lv.u.New.nodes [i] = (NODE *) sxcalloc ((SXUINT)j, (SXUINT)sxatcvar.atc_lv.node_size);
}

static NODE	*allocate_area (void)
{
    struct sxatc_area		*old, *new;

    old = sxatcvar.atc_lv.u.Old.area;
    sxatcvar.atc_lv.u.Old.area = new = (struct sxatc_area*) sxcalloc (1, sizeof (struct sxatc_area) - sizeof (NODE) + NEW_AREA_SIZE);
    new->next = old;
    sxatcvar.atc_lv.u.Old.area_end = (char*) new->zone + (sxatcvar.atc_lv.u.Old.area_size - sxatcvar.atc_lv.node_size) /
	 sizeof (char);
    sxatcvar.atc_lv.u.Old.current_node = new->zone;
    return new->zone;
}



static void nodes_alloc (void)
{
    SXINT i, j;
    BOOLEAN first_time = TRUE;

    if (sxindex_is_allocated(sxatcvar.atc_lv.u.New.index_hd)) {
      first_time = FALSE;
      sxindex_clear (sxatcvar.atc_lv.u.New.index_hd);
    }
    else
      sxindex_alloc (&sxatcvar.atc_lv.u.New.index_hd, 2, 511, sxatc_oflw);

    sxatcvar.atc_lv.u.New.line_nb = i = sxindex_line_nb (sxatcvar.atc_lv.u.New.index_hd);

    if (first_time)
      sxatcvar.atc_lv.u.New.nodes = (NODE **) sxalloc (i, sizeof (NODE *));

    j = sxindex_column_nb (sxatcvar.atc_lv.u.New.index_hd);

    if (first_time)
      while (--i >= 0)
	memset (sxatcvar.atc_lv.u.New.nodes [i], 0, /* (size_t) (*/ (SXUINT)j * (SXUINT)sxatcvar.atc_lv.node_size);
    else
      while (--i >= 0)
	sxatcvar.atc_lv.u.New.nodes [i] = (NODE *) sxcalloc ((SXUINT)j, (SXUINT)sxatcvar.atc_lv.node_size);
}

static VOID	freeareas (void)
{
    struct sxatc_area		*p, *q;

    q = sxatcvar.atc_lv.u.Old.area;

    while ((p = q) != NULL) {
	q = p->next;
	sxfree (p);
    }
}



static void free_tree (void)
{
    SXINT i = sxatcvar.atc_lv.u.New.line_nb;

    while (--i >= 0)
	sxfree (sxatcvar.atc_lv.u.New.nodes [i]);
}

static NODE *allocate_new_node (void)
{
    SXINT		x;
    NODE	*p;

    x = sxindex_seek (&(sxatcvar.atc_lv.u.New.index_hd));
    p = (NODE *)
       (sxatcvar.atc_lv.u.New.nodes [sxindex_i (sxatcvar.atc_lv.u.New.index_hd, x)] +
        sxindex_j (sxatcvar.atc_lv.u.New.index_hd, x) );
    p->index = x;
    return p;
}

void sub_tree_erase (NODE *node)
{
#include <memory.h>

    NODE	*brother = node->son, *son;
    SXINT		sub_tree_erase_index;

    while ((son = brother) != NULL) {
	sub_tree_erase (son);
	sub_tree_erase_index = son->index;
	brother = son->brother;
	memset ((char*) son, 0, (size_t) sxatcvar.atc_lv.node_size); /* RAZ node */
	sxindex_release (sxatcvar.atc_lv.u.New.index_hd, sub_tree_erase_index);
    }
}


void sxatc_sub_tree_erase (/* NODE *node */ struct sxnode_header_s *node)
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

static NODE	*brother, *son;
static SXINT	top, new_top;



static NODE	*rightmost_son (NODE *node_ptr)
{
    NODE	*next;

    while ((next = node_ptr->son) != NULL) {
	while ((next = (node_ptr = next)->brother) != NULL) {
	    /* VOID */
	}
    }

    return node_ptr;
}



static char	*string_cat (char *s1, char *s2)
{
    size_t	l1;
    char	*s;

    s = sxrealloc (s1, (l1 = strlen (s1)) + strlen (s2) + 1, sizeof (char));
    strcat (s + l1, s2);
    sxfree (s2);
    return s;
}



static VOID	cons_comments (BOOLEAN is_right_cons, NODE *node_ptr, SXINT low, SXINT high)
{
    char	*com, *com1;
    SXINT	x;

    for (com = NULL, x = low; x <= high; x++) {
	if (STACKtoken (x).lahead >= 0 /* not an action */  && (com1 = STACKtoken (x).comment) != NULL)
	    com = (com == NULL) ? com1 : string_cat (com, com1);
    }

    if (com != NULL)
	node_ptr->token.comment = (com1 = node_ptr->token.comment) != NULL ? (is_right_cons ? string_cat (com1, com) : string_cat (com, com1)) : com;
	
}



static VOID	initialize (void)
     /* struct sxtables	*tables; */
/* On traite un nouveau texte source */
{
    sxatcvar.atc_lv.abstract_tree_root = NULL;
    sxatcvar.atc_lv.abstract_tree_is_error_node = FALSE;

    if (reccall++ == 0)
	stacktreenode = (NODE**) sxalloc ((stns = stns_incr) + 1, sizeof (NODE*));

    if (sxatcvar.atc_lv.early_visit_set == NULL) {
	sxatcvar.atc_lv.u.Old.area = NULL;
	sxatcvar.atc_lv.u.Old.current_node = sxatcvar.atc_lv.u.Old.area_end = NULL;
    }
    else
	nodes_alloc ();
}



static VOID	finalize (void)
     /* struct sxtables	*tables; */
/* on a fini de traiter un texte source */
{
    if (!sxatcvar.atc_lv.are_comments_erased) {
	/* traitement du commentaire associe a EOF */

	if (sxsvar.lv.terminal_token.comment != NULL) {
	    NODE	*p;

	    p = rightmost_son (sxatcvar.atc_lv.abstract_tree_root);
	    p->token.comment = (p->token.comment == NULL ? sxsvar.lv.terminal_token.comment : string_cat (p->token.
		 comment, sxsvar.lv.terminal_token.comment));
	}
    }

    if (sxatcvar.atc_lv.abstract_tree_root != NULL)
	sxatcvar.atc_lv.abstract_tree_root->father = NULL /* in case the root is a list */ ;

#ifdef SXRESIZE
    if (sxatcvar.atc_lv.early_visit_set == NULL && sxatcvar.atc_lv.u.Old.area != NULL) {
	sxatcvar.atc_lv.u.Old.area_end = sxatcvar.atc_lv.u.Old.current_node;
	sxatcvar.atc_lv.u.Old.area_size = ((SXNODE*)sxatcvar.atc_lv.u.Old.area_end - (SXNODE*) sxatcvar.atc_lv.u.Old.area->zone)
	  * sizeof (SXNODE) + sxatcvar.atc_lv.node_size;
	SXRESIZE (sxatcvar.atc_lv.u.Old.area, 1, sizeof (struct sxatc_area) - sizeof (NODE) + sxatcvar.atc_lv.u.Old.area_size);
    }
#endif

    if (--reccall == 0)
	sxfree (stacktreenode);
}



static VOID	test_and_create_son (SXINT x)
{
/* x reference soit un nt soit un t generique */
    struct sxtoken	*token = &(STACKtoken (x));

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






static VOID	c_family (void)
{
    SXINT	i, bsup, pos;
    SXINT	binf, low;

    bsup = sxatcvar.SXT_tables.SXT_node_info [STACKreduce () + 1].T_ss_indx;
    binf = sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_ss_indx;
    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    sxatcvar.atc_lv.abstract_tree_root->name = sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_node_name;
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
	sxatcvar.atc_lv.abstract_tree_root->token.lahead = STACKtoken (new_top).lahead;

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	low = new_top;

	if (bsup > binf) {
	    BOOLEAN	is_first = TRUE;
	    NODE	*node_ptr = sxatcvar.atc_lv.abstract_tree_root;

	    for (i = bsup - 1; i >= binf; i--) {
		SXINT	j;

		if (is_first) {
		    is_first = FALSE;
		    node_ptr = node_ptr->son;
		}
		else
		    node_ptr = node_ptr->brother;

		if ((j = top - sxatcvar.SXT_tables.T_stack_schema [i]) > low)
		    cons_comments (FALSE, node_ptr, low, j - 1);

		low = j + 1;
	    }

	    if (top >= low)
		cons_comments (TRUE, rightmost_son (node_ptr), low, top);
	}
	else if (top >= low)
	    cons_comments (FALSE, sxatcvar.atc_lv.abstract_tree_root, low, top);
    }

    sxatcvar.atc_lv.abstract_tree_root->token.source_index = STACKtoken (new_top).source_index;
}



static VOID	add_right_list_node (void)
{
    SXINT	i, low, pos, high;
    NODE	*node_ptr;

    i = sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_ss_indx;
    high = top - sxatcvar.SXT_tables.T_stack_schema [i];
    low = top - sxatcvar.SXT_tables.T_stack_schema [i + 1];
    sxatcvar.atc_lv.abstract_tree_root = stacktreenode [high];
    son = sxatcvar.atc_lv.abstract_tree_root->son;
    test_and_create_son (low);
    node_ptr = sxatcvar.atc_lv.abstract_tree_root->son = brother;
    node_ptr->brother = son;
    node_ptr->first_list_element = TRUE;

    if (son != NULL)
	son->first_list_element = FALSE;
    else
	node_ptr->last_list_element = TRUE;

    pos = 0;

    while (brother != NULL) {
	brother->position = ++pos;
	brother = brother->brother;
    }

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (low > new_top)
	    cons_comments (FALSE, node_ptr, new_top, low - 1);

	if (--high > low)
	    cons_comments (TRUE, rightmost_son (node_ptr), low + 1, high);
    }

    sxatcvar.atc_lv.abstract_tree_root->token.source_index = STACKtoken (new_top).source_index;
}



static VOID	add_list_node (void)
{
    SXINT	i, low, high;

    i = sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_ss_indx;
    high = top - sxatcvar.SXT_tables.T_stack_schema [i];
    low = top - sxatcvar.SXT_tables.T_stack_schema [i + 1];
    sxatcvar.atc_lv.abstract_tree_root = stacktreenode [low];
    son = sxatcvar.atc_lv.abstract_tree_root->father;
    /* the most RHS son */

    test_and_create_son (high);

    if (son == NULL) {
	sxatcvar.atc_lv.abstract_tree_root->son = brother;
	brother->first_list_element = TRUE;
    }
    else {
	son->brother = brother;
	son->last_list_element = FALSE;
    }

    brother->position = ++sxatcvar.atc_lv.abstract_tree_root->degree;
    brother->last_list_element = TRUE;
    sxatcvar.atc_lv.abstract_tree_root->father = brother;


/* new most RHS son */

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (--high >= ++low)
	    cons_comments (FALSE, brother, low, high);

	if (--high >= ++low)
	    cons_comments (TRUE, rightmost_son (brother), low, top);
    }
}



static VOID	create_error_family (void)
{
    SXINT	j, pos, low, lahead;
    struct sxtoken	*tok;
    NODE	*node_ptr;

    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    sxatcvar.atc_lv.abstract_tree_root->name = ERROR_n;
    sxatcvar.atc_lv.abstract_tree_root->is_list = TRUE;
    pos = 0;
    low = new_top;

    for (j = new_top; j <= top; j++) {
	/* j = parse_stack index */
	lahead = (tok = &(STACKtoken (j)))->lahead;

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
		node_ptr->first_list_element = TRUE;
	    }
	    else
		son->brother = node_ptr;

	    son = node_ptr;

	    if (!sxatcvar.atc_lv.are_comments_erased) {
		if (j > low)
		    cons_comments (FALSE, node_ptr, low, j - 1);

		low = j + 1;
	    }
	}
    }

    if (pos != 0) {
	son->last_list_element = TRUE;
	sxatcvar.atc_lv.abstract_tree_root->father = son /* ptr to the most RHS son */ ;

	if (!sxatcvar.atc_lv.are_comments_erased) {
	    if (top >= low)
		cons_comments (TRUE, son, low, top);
	}
    }
    else {
	if (!sxatcvar.atc_lv.are_comments_erased && top >= low)
	    cons_comments (FALSE, sxatcvar.atc_lv.abstract_tree_root, low, top);
    }

    sxatcvar.atc_lv.abstract_tree_root->degree = pos;
    sxatcvar.atc_lv.abstract_tree_root->token.source_index = STACKtoken (new_top).source_index;
}



static VOID	c_leaf (void)
{
    SXINT	x;

    sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
    x = top - sxatcvar.SXT_tables.T_stack_schema [sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_ss_indx];
    sxatcvar.atc_lv.abstract_tree_root->token = STACKtoken (x);
    sxatcvar.atc_lv.abstract_tree_root->name = sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_node_name;

    if (!sxatcvar.atc_lv.are_comments_erased) {
	if (x > new_top)
	    cons_comments (FALSE, sxatcvar.atc_lv.abstract_tree_root, new_top, x - 1);

	if (top > x)
	    cons_comments (TRUE, sxatcvar.atc_lv.abstract_tree_root, x + 1, top);
    }
}



static VOID	atc_open (struct sxtables *tables)
                   	        
/* On traite un nouveau langage */
{
    sxatcvar.SXT_tables = tables->sem_tables->SXT_tables;
    sxatcvar.atc_lv.node_size = sizeof (NODE) /* default value for dummy smp */ ;
    sxatcvar.atc_lv.are_comments_erased = tables->SXS_tables.S_are_comments_erased;
    sxatcvar.atc_lv.early_visit_set = NULL;
    (*sxatcvar.SXT_tables.sempass) (OPEN, tables);

    if (sxatcvar.atc_lv.early_visit_set == NULL)
	sxatcvar.atc_lv.u.Old.area_size = 0;
}



static VOID	action (SXINT act)
{
    SXINT	j;

    top = STACKtop ();
    new_top = STACKnewtop ();

    /* Dans tous les cas (meme production vide ou action), "STACKtoken (new_top).source_index"
       est positionne' correctement par le parser. */

    if (top >= stns) {
	stacktreenode = (NODE**) sxrealloc (stacktreenode, (stns = top + stns_incr) + 1, sizeof (NODE*));
    }

    switch (act) {
    case 0:
	j = top - sxatcvar.SXT_tables.T_stack_schema [sxatcvar.SXT_tables.SXT_node_info [STACKreduce ()].T_ss_indx];
	sxatcvar.atc_lv.abstract_tree_root = stacktreenode [j];

	if (!sxatcvar.atc_lv.are_comments_erased) {
	    if (j > new_top)
		cons_comments (FALSE, sxatcvar.atc_lv.abstract_tree_root, new_top, j - 1);

	    if (top > j)
		cons_comments (TRUE, rightmost_son (sxatcvar.atc_lv.abstract_tree_root), j + 1, top);
	}

	if (j > new_top)
	    /* Il y a des terminaux non-generiques plus a gauche */
	    sxatcvar.atc_lv.abstract_tree_root->token.source_index = STACKtoken (new_top).source_index;

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
	sxatcvar.atc_lv.abstract_tree_root->is_list = TRUE;

	if ((son = sxatcvar.atc_lv.abstract_tree_root->son) != NULL) {
	    son->first_list_element = TRUE;
	    brother->last_list_element = TRUE;
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



VOID	sxatc (SXINT sxatc_what, struct sxtables *arg)
    /* si sxatc_what=ACTION, arg est un entier entre 0 et 6 ;  si sxatc_what != ACTION, arg est un pointeur vers des tables. */
{
    long l;

    switch (sxatc_what) {
    case OPEN:
	sxcheck_magic_number (arg->magic, SXMAGIC_NUMBER, "atc");
	atc_open (arg);
	break;

    case INIT:
      initialize (/* arg */);
	break;

    case FINAL:
      finalize (/* arg */);
	break;

    case ACTION:
      /* puisque sxatc_what=ACTION, arg n'est pas un pointeur vers des tables, mais bien un entier entre 0
	 et 6, on veut que le compilo C ne hurle pas sur un probl�me de typage, d'o� le(s?) cast(s?) */
	/* LINTED this cast is harmless according to Atoll (cf previous line comment) */ 
	action ((SXINT) (long) arg);

	/* pbs sur les listes..., appel sur chaque nouveau fils. */
	if (sxatcvar.atc_lv.early_visit_set != NULL &&
	    SXBA_bit_is_set (sxatcvar.atc_lv.early_visit_set, sxatcvar.atc_lv.abstract_tree_root->name))
	    (*sxatcvar.SXT_tables.sempass) (SEMPASS, arg);

	break;

    case ERROR:
	sxatcvar.atc_lv.abstract_tree_is_error_node = TRUE;

	if ((l = (long) arg) > 0) {
	    action ((SXINT)4);

	    while (--l > 0) {
		if (++new_top >= stns)
		    stacktreenode = (NODE**)
			sxrealloc (stacktreenode, (stns = new_top + stns_incr) + 1, sizeof (NODE*));

		sxatcvar.atc_lv.abstract_tree_root = allocate_node ();
		sxatcvar.atc_lv.abstract_tree_root->name = ERROR_n;
		sxatcvar.atc_lv.abstract_tree_root->is_list = TRUE;
		stacktreenode [new_top] = sxatcvar.atc_lv.abstract_tree_root;
	    }
	}

	break;

    case SEMPASS:
	(*sxatcvar.SXT_tables.sempass) (ACTION, arg);
	break;

    case CLOSE:
	free_node ();
	(*sxatcvar.SXT_tables.sempass) (CLOSE, arg);
	break;

    default:
	fprintf (sxstderr, "The function \"sxatc\" is out of date with respect to its specification.\n");
	sxexit(1);
    }
}
