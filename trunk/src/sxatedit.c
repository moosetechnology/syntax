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

/* module pour ecrire des arbres abstraits standards (struct sxnode_header_s)
   sur un fichier.
*/

#include "sxversion.h"
#include "sxunix.h"

char WHAT_SXATEDIT[] = "@(#)SYNTAX - $Id: sxatedit.c 2947 2023-03-29 17:06:41Z garavel $" WHAT_DEBUG;

#define USER_PTR	void*		/* Any pointer, same as in sxpptree.c */

static FILE	*ateditfile = NULL;
static SXINT	min_dist = 1, min_dist_on_subtrees = 1, page_width = 132;
extern SXVOID	sxpptree (USER_PTR tree,
			  SXINT min_dist_sxpptree,
			  SXINT min_dist_on_subtrees_sxpptree,
			  SXINT page_width_sxpptree,
			  SXINT (*get_key_length) (USER_PTR),
			  USER_PTR (*get_left_son) (USER_PTR),
			  USER_PTR (*get_right_brother) (USER_PTR),
			  SXINT (*get_sons_number) (USER_PTR),
			  SXVOID (*print_from_arrows) (char *),
			  SXVOID (*print_horizontal_bar) (char *),
			  SXVOID (*print_key) (USER_PTR, SXINT, SXINT),
			  SXVOID (*print_to_arrows) (char *),
			  SXVOID (*put_blanks) (SXINT),
			  SXVOID (*put_page) (void),
			  SXVOID (*put_skip) (void),
			  SXINT switches);



SXVOID	sxateditinit (FILE *file, SXINT md1, SXINT md2, SXINT pw)
        	      
        	             
/* le fichier de sortie (passe en parametre) doit avoir ete ouvert
   precedemment.
*/
{
    ateditfile = file;
    min_dist = md1;
    min_dist_on_subtrees = md2;
    page_width = pw;
}



static SXINT	get_key_length (SXNODE *node)
        	      
/* rend le nombre de caracteres que l'on veut imprimer comme information
   relative au noeud passe en argument.
   Noeud "non-terminal" ==> nom du noeud
   Noeud "terminal" ==> texte du token
*/
{
    if (node->son == NULL && node->token.string_table_entry != 0)
	/* noeud terminal */
	return (SXINT) sxstrlen (node->token.string_table_entry);
    else
	/* noeud non-terminal */
	return (SXINT) strlen (sxatcvar.SXT_tables.T_node_name [node->name]);
}



static SXNODE	*get_left_son (SXNODE *node)
        	      
/* rend un pointeur vers le premier fils (fils de gauche) du noeud
   passe en argument. */
{
    return node->son;
}



static SXNODE	*get_right_brother (SXNODE *node)
        	      
/* rend un pointeur vers le frere de droite du noeud passe en argument. */
{
    return node->brother;
}



static SXINT	get_sons_number (SXNODE *node)
        	      
/* rend le nombre de fils du noeud passe en argument. */
{
    return (SXINT) node->degree;
}



static SXVOID	print_a_string (char *str)
        	     
/* imprime la chaine de caracteres passee en argument, suivie d'une fin de
   ligne. Utilisee pour imprimer les arcs et les barres horizontales. */
{
    fputs (str, ateditfile);
    putc ('\n', ateditfile);
}



static SXVOID	print_key (SXNODE *node, SXINT start, SXINT lgt)
        	      
        	      
        	    
/* imprime une partie de l'information relative au noeud passe en premier
   argument; le second argument donne l'indice du premier caractere a sortir
   (compte a partir de 1), le dernier etant le nombre de caracteres a ecrire.
*/
{
    char	*str;

    if (node->son == NULL && node->token.string_table_entry != 0)
	/* noeud terminal */
	str = sxstrget (node->token.string_table_entry);
    else
	/* noeud non-terminal */
	str = sxatcvar.SXT_tables.T_node_name [node->name];


/* positionnons-nous au debut de la partie a ecrire */

    str += start - 1;

    if ((size_t)lgt == strlen (str))
	/* imprimer toute la chaine ou tout le reste de la chaine */
	fputs (str, ateditfile);
    else
	/* imprimer exactement le nombre de caracteres demande */
	for (; lgt > 0; lgt--)
	    putc (*str++, ateditfile);
}



static SXVOID	put_blanks (SXINT count)
        	      
/* imprime le nombre d'espaces indiques */
{
    for (; count > 0; count--)
	putc (' ', ateditfile);
}



static SXVOID	put_page (void)
/* se repositionne sur la page suivante, pour commencer un nouveau le qui
   devra etre place a cote du precedent. */
{
    putc ('\f', ateditfile);
}



static SXVOID	put_skip (void)
/* se positionne en colonne un sur la ligne qui suit. */
{
    putc ('\n', ateditfile);
}



SXVOID	sxatedit (SXNODE *tree)
        	      
/* fait le boulot! */
{

#define print_from_arrows	print_a_string
#define print_to_arrows		print_a_string
#define print_horizontal_bar	print_a_string
#define switches		(SXINT) 0

    sxpptree ((USER_PTR) tree,
	      min_dist,
	      min_dist_on_subtrees, 
	      page_width, 
	      (SXINT  (*) (USER_PTR))   get_key_length, 
	      (USER_PTR(*) (USER_PTR)) get_left_son, 
	      (USER_PTR(*) (USER_PTR)) get_right_brother,
	      (SXINT  (*) (USER_PTR))   get_sons_number, 
	      print_from_arrows, 
	      print_horizontal_bar, 
	      (void (*) (USER_PTR, SXINT, SXINT)) print_key, 
	      print_to_arrows, 
	      put_blanks, 
	      put_page,
	      put_skip, 
	      switches);
}
