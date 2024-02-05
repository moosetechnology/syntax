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

/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * */
/*									*/
/*									*/
/* Mise en page et impression d'un arbre quelconque.			*/
/* -------------------------------------------------			*/
/*									*/
/* L'arbre est imprime sous forme de lés, chaque lé ayant pour largeur	*/
/* la valeur de la variable "page_width".  La distance minimale entre	*/
/* deux noeuds contigus sur une meme ligne est donnee par la valeur de	*/
/* "min_dist".  La distance minimale entre deux sous-arbres cousins est */
/* donnee par la valeur de "min_dist_on_subtrees".  Les valeurs par	*/
/* defaut de ces trois variables sont respectivement 132, 3 et min_dist.*/
/*									*/
/*  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  *  * */


#include        "sxversion.h"
#include        "sxcommon.h"
#include <stdio.h>
#include <stdlib.h>

char WHAT_SXPPTREE[] = "@(#)SYNTAX - $Id: sxpptree.c 3633 2023-12-20 18:41:19Z garavel $" WHAT_DEBUG;

#define USER_PTR	void*		/* Any pointer */

struct positions {
    SXINT	left, middle, right;
    SXINT	l_son_middle, r_son_middle;
};
struct informations_on_user_node {
    USER_PTR	user_node;
    SXINT		key_start;
    SXINT		key_length;
    SXINT		sons_number;
};
struct node {
    struct node		*next, *l_son_node;
    struct informations_on_user_node	info;
    struct positions	pos;
};
struct line {
    struct line		*next;
    struct node		*first, *last;
    SXINT	h_bar_ovfl;
};
static SXINT	max_level;
/* nb of levels in the tree when in
					   abbrev-mode */


static SXINT	page_length;
/* nb of lines in the page when in
					   abbrev-mode */


static struct line	*lines = {NULL};
static SXINT	minpos, maxpos;
static struct {
	   SXINT		min_dist;
	   SXINT		min_dist_on_subtrees;
	   SXINT		page_width;
	   SXINT		(*get_key_length) (USER_PTR);
	   USER_PTR	(*get_left_son) (USER_PTR);
	   USER_PTR	(*get_right_brother) (USER_PTR);
	   SXINT		(*get_sons_number) (USER_PTR);
	   void		(*print_from_arrows) (char *);
	   void		(*print_horizontal_bar) (char *);
	   void		(*print_key) (USER_PTR, SXINT, SXINT);
	   void		(*print_to_arrows) (char *);
	   void		(*put_blanks) (SXINT);
	   void		(*put_page) (void);
	   void		(*put_skip) (void);
	   bool	abbrev_mode /* on if the tree may be abbreviated */ ;
       }	args;



static void	shift_subtree (struct node *node, SXINT s)
{
    struct node	*son;
    SXINT	count;

    node->pos.left += s, node->pos.middle += s, node->pos.right += s;
    node->pos.l_son_middle += s, node->pos.r_son_middle += s;
    son = node->l_son_node;

    if (!args.abbrev_mode || son != NULL)
	for (count = node->info.sons_number; count-- > 0; ) {
	    shift_subtree (son, s);
	    son = son->next;
	}
}



static void	init_h_bar_ovfl (void)
{
    struct line	*line;

    for (line = lines; line != NULL; line = line->next) {
	line->h_bar_ovfl = 0;
    }
}



static void	min_max_pos (void)
{
    struct line	*line;
    SXINT	min, max;

    line = lines;
    min = line->first->pos.left, max = line->last->pos.right;
    line->last = NULL;

    while ((line = line->next) != NULL) {
	SXINT	val;

	if ((val = line->first->pos.left) < min)
	    min = val;

	if ((val = line->last->pos.right) > max)
	    max = val;

	line->last = NULL;
    }

    minpos = min, maxpos = max;
}




#include <setjmp.h>

static jmp_buf	no_space;




#ifdef SIMPLE

static void	*allocate (size)
    SXINT		size;
{
    void	*result;

    if ((result = malloc (size)) == NULL)
	longjmp (no_space, true);

    return result;
}




#define	node_dispose(node)	free (node)
#define	line_dispose(line)	free (line)

static void	lines_destroy (first_line)
    struct line		*first_line;
{
    struct node	*node, *next_node;
    struct line	*line, *next_line;

    for (line = first_line; line != NULL; line = next_line) {
	next_line = line->next;

	for (node = line->first; node != NULL; node = next_node) {
	    next_node = node->next;
	    node_dispose (node);
	}

	line_dispose (line);
    }
}




#define	deallocate_areas()	0

#else

#define	NBLINES	10		/* number of lines allocated together */
#define	NBNODES	100		/* number of nodes allocated together */

static struct line_area {
	   struct line_area	*next;
	   struct line	array [NBLINES];
       }	*first_line_area;
static struct node_area {
	   struct node_area	*next;
	   struct node	array [NBLINES];
       }	*first_node_area;



static void	*allocate (SXUINT size)
{
    static struct line_area	*line_area;
    static struct line	*last_line;
    static struct node_area	*node_area;
    static struct node	*last_node;

    switch (size) {
    case sizeof (struct line):
	if (first_line_area == NULL || last_line == line_area->array) {
	    struct line_area	*new;

	    new = (struct line_area*) allocate (sizeof (struct line_area));
	    new->next = NULL;

	    if (first_line_area == NULL)
		first_line_area = new;
	    else
		line_area->next = new;

	    last_line = &((line_area = new)->array [NBLINES]);
	}

	return --last_line;

    case sizeof (struct node):
	if (first_node_area == NULL || last_node == node_area->array) {
	    struct node_area	*new;

	    new = (struct node_area*) allocate (sizeof (struct node_area));
	    new->next = NULL;

	    if (first_node_area == NULL)
		first_node_area = new;
	    else
		node_area->next = new;

	    last_node = &((node_area = new)->array [NBLINES]);
	}

	return --last_node;

    default:
	{
	    void	*result;

	    if ((result = malloc (size)) == NULL)
		longjmp (no_space, true);

	    return result;
	}
    }
}




#define	node_dispose(node)	
#define	line_dispose(line)	
#define	lines_destroy(line)	


static void	deallocate_areas (void)
{
    struct line_area	*line_area;
    struct node_area	*node_area;

    for (line_area = first_line_area; line_area != NULL; line_area = line_area->next)
	free (line_area);

    first_line_area = NULL;

    for (node_area = first_node_area; node_area != NULL; node_area = node_area->next)
	free (node_area);

    first_node_area = NULL;
}




#endif /* SIMPLE */



static void	trim_nodes (struct node *node)
{
    while (node != NULL) {
	node->l_son_node = NULL;
	node = node->next;
    }
}



static void	trim_lines (struct line *line, SXINT level)
{
    while (level < max_level && line != NULL) {
	line = line->next;
	level++;
    }

    if (line != NULL) {
	trim_nodes (line->first);
	lines_destroy (line->next);
	line->next = NULL;
    }
}



static void	place (USER_PTR user_ptr, struct line **line_ptr, SXINT level, bool is_left_son)
{
    struct line	*line;
    struct node	*node;
    SXINT	left_pos;
    SXINT		current_size, cursize, half_size;
    bool	first_node_on_this_line_p;

    half_size = (cursize = (current_size = args.get_key_length (user_ptr)) - 1) >> 1;

/* On cree un nouveau node vers user_ptr */

    node = (struct node*) allocate (sizeof (struct node));
    node->next = node->l_son_node = NULL;
    node->info.user_node = user_ptr;
    node->info.key_start = 1;
    node->info.key_length = current_size;
    node->info.sons_number = args.get_sons_number (user_ptr);
    first_node_on_this_line_p = (*line_ptr) == NULL;

    if (first_node_on_this_line_p) {
	(*line_ptr) = line = (struct line*) allocate (sizeof (struct line));
	line->next = NULL;
	line->first = node;
	line->h_bar_ovfl = 0;
	left_pos = 0;
    }
    else {
	line = *line_ptr;
	line->last->next = node;
	left_pos = line->last->pos.right + 1;

	{
	    SXINT	val = is_left_son ? args.min_dist_on_subtrees : args.min_dist;

	    line->h_bar_ovfl += val;
	    left_pos += val;
	}
    }

    line->last = node;
    line->h_bar_ovfl += current_size;

    if (args.abbrev_mode && line->h_bar_ovfl > args.page_width)
	max_level = level;


/* On ajuste la position en fonction des positions des fils eventuels */

    if (node->info.sons_number == 0 || (args.abbrev_mode && level >= max_level)) {
	node->pos.left = left_pos;
	node->pos.right = left_pos + cursize;
	node->pos.middle = node->pos.l_son_middle = node->pos.r_son_middle = left_pos + half_size;
    }
    else {
	bool		left_son_first_on_his_line_p = line->next == NULL;

	level++;

	{
	    USER_PTR	son;
	    SXINT	count;

	    place (son = args.get_left_son (user_ptr), &(line->next), level, true);
	    node->l_son_node = line->next->last;

	    for (count = node->info.sons_number; --count > 0; )
		place (son = args.get_right_brother (son), &(line->next), level, false);
	}


/* On le place au milieu de ses fils, en les recadrant si necessaire */

	node->pos.l_son_middle = node->l_son_node->pos.middle;
	node->pos.r_son_middle = line->next->last->pos.middle;
	node->pos.middle = (node->pos.l_son_middle + node->pos.r_son_middle + 1) >> 1;
	node->pos.left = node->pos.middle - half_size;
	node->pos.right = node->pos.left + cursize;

	if (!first_node_on_this_line_p) {
	    SXINT	shift_q = left_pos - node->pos.left;

	    if (shift_q != 0)
		if (shift_q > 0 || left_son_first_on_his_line_p)
		    shift_subtree (node, shift_q);
	}
    }
}



static void	replace (struct line *line, SXINT level, bool is_left_son, bool first_time_on_this_level)
{
    struct node		*node;
    SXINT	left_pos;
    SXINT		sons_nb;
    SXINT		cursize, half_size;
    bool	first_node_on_this_line_p;

    first_node_on_this_line_p = line->last == NULL;

    if (first_node_on_this_line_p) {
	node = line->first;
	left_pos = 0;
    }
    else {
	node = line->last->next;
	left_pos = line->last->pos.right + 1 + (is_left_son ? args.min_dist_on_subtrees : args.min_dist);
    }

    line->last = node;

/* On ajuste la position en fonction des positions des fils eventuels */

    sons_nb = node->info.sons_number;

    if (sons_nb == 0 || level == max_level) {
	if (level == max_level)
	    if ((first_time_on_this_level && sons_nb != 0) || (!first_time_on_this_level && sons_nb == 0)) {
		node->info.key_length = args.get_key_length (node->info.user_node);
		node->info.key_start = -1;
	    }

	half_size = (cursize = node->info.key_length - 1) >> 1;
	node->pos.left = left_pos;
	node->pos.right = left_pos + cursize;
	node->pos.middle = node->pos.l_son_middle = node->pos.r_son_middle = left_pos + half_size;
    }
    else {
	bool		left_son_first_on_his_line_p = line->next->last == NULL;

	level++;

	{
	    SXINT	count;

	    replace (line->next, level, true, first_time_on_this_level);

	    for (count = sons_nb; --count > 0; )
		replace (line->next, level, false, first_time_on_this_level);
	}


/* On le place au milieu de ses fils, en les recadrant si necessaire */

	node->pos.l_son_middle = node->l_son_node->pos.middle;
	node->pos.r_son_middle = line->next->last->pos.middle;

	{
	    SXINT	val = ((node->pos.l_son_middle + node->pos.r_son_middle + 1) >> 1) - node->pos.middle;

	    node->pos.left += val, node->pos.middle += val, node->pos.right += val;
	}

	if (!first_node_on_this_line_p) {
	    SXINT	shift_q = left_pos - node->pos.left;

	    if (shift_q != 0)
		if (shift_q > 0 || left_son_first_on_his_line_p)
		    shift_subtree (node, shift_q);
	}
    }
}



static void	abbreviate_tree (void)
{
    bool	done;
    /* is the tree abbreviated enough */
    bool	first_time_on_this_level;

    /* we recompute each level
						   twice if necessary */

    trim_lines (lines, (SXINT)1);
    /* as max_level may have been updated */
    min_max_pos ();
    args.put_page ();
    /* from now on, get abbreviations */
    first_time_on_this_level = true;
    done = max_level <= 1 || maxpos - minpos < args.page_width;

    while (!done) {
	replace (lines, (SXINT)1, true, first_time_on_this_level);
	trim_lines (lines, (SXINT)1);
	min_max_pos ();

	if (!(done = max_level <= 1 || maxpos - minpos < args.page_width)) {
	    if ((first_time_on_this_level = !first_time_on_this_level))
		max_level--;
	}
    }

    maxpos = minpos + args.page_width - 1;
}



static SXINT	start_pos, next_stripe, current_pos;
static SXINT	line_no;
/* number of lines output when in
					   abbrev-mode */


static struct line	*line;
static struct string {
	   char		*text;
	   SXINT	length;
       }	from_arrows, to_arrows, horizontal_bar;



static void	catenate_copy (struct string *string, char character, SXINT number)
{
    char	*str;

    str = &(string->text [string->length]);
    string->length += number;

    while (number-- > 0)
	*str++ = character;
}



static void	rateau (void)
{
    SXINT	pos;
    /* from_arrows.length =
					   to_arrows.length = pos - start_pos */
    struct node	*node;

    horizontal_bar.length = to_arrows.length = from_arrows.length = 0;

    if (line->h_bar_ovfl > 0) {
	SXINT	length;

	line->h_bar_ovfl -= length = line->h_bar_ovfl > args.page_width ? args.page_width : line->h_bar_ovfl;
	catenate_copy (&horizontal_bar, '-', length);
    }

    pos = start_pos;

    for (node = line->first; node != NULL && pos <= next_stripe; node = node->next) {
      if (node->info.sons_number > 0)
	if (node->pos.l_son_middle < next_stripe && node->pos.l_son_middle >= start_pos)
	  /* pas encore traite */
	  /* prepare_horizontal_bar:
		(pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */
	  {
	    catenate_copy (&horizontal_bar, ' ', node->pos.l_son_middle - start_pos - horizontal_bar.length);

	    if (node->pos.r_son_middle >= next_stripe) {
	      catenate_copy (&horizontal_bar, '-', args.page_width - horizontal_bar.length);
	      line->h_bar_ovfl = node->pos.r_son_middle - next_stripe + 1;
	    }
	    else if (node->info.sons_number > 1)
	      catenate_copy (&horizontal_bar, '-', node->pos.r_son_middle - node->pos.l_son_middle + 1);
	    else
	      horizontal_bar.text [horizontal_bar.length++] = '|';
	  }

      if (node->pos.middle < next_stripe) {
	if (node->pos.middle >= start_pos) {
	  /* pas encore traite */
	  /* prepare_arrows: 
		(pas de goto sur ce label, ce qui provoque un warning du compilo icc d'intel) */
	  {
	    SXINT	spaces_nb = node->pos.middle - pos;

	    catenate_copy (&from_arrows, ' ', spaces_nb);
	    from_arrows.text [from_arrows.length++] = '|';
	    catenate_copy (&to_arrows, ' ', spaces_nb);
	    to_arrows.text [to_arrows.length++] = node->info.sons_number != 0 ? '|' : ' ';
	  }

	  pos = node->pos.middle + 1;
	}
      }
      else if (node->info.sons_number > 0)
	/* sinon, on traite les freres */
	pos = node->pos.middle + 1;
    }
}



static void	print_one_node (struct node *node)
               	      

/* Si la cle tient sur la ligne, on l'imprime; */
/* sinon on en imprime ce qu'on peut et on     */
/* garde le reste pour le prochain le.	       */

{
    SXINT	current_key_length, remaining_space;

    if (node->pos.left > current_pos)
	args.put_blanks (node->pos.left - current_pos);

    current_pos = node->pos.right + 1;

    if ((current_key_length = node->info.key_length) <= (remaining_space = next_stripe - node->pos.left)) {
	args.print_key (node->info.user_node, node->info.key_start, current_key_length);
	line->first = node->next;
	node_dispose (node);
    }
    else {
	args.print_key (node->info.user_node, node->info.key_start, remaining_space);
	node->info.key_start += remaining_space;
	node->info.key_length -= remaining_space;
	node->pos.left = next_stripe;
    }
}



static void	print_one_line (void)
{
    struct node	*node;

    rateau ();

    if (line != lines) {
	from_arrows.text [from_arrows.length] = '\0';
	args.print_from_arrows (from_arrows.text);
	line_no++;
    }
    else if (!args.abbrev_mode)
	args.print_from_arrows ("");

    current_pos = start_pos;

    while (current_pos < next_stripe && (node = line->first) != NULL) {
	if (node->pos.left < next_stripe)
	    print_one_node (node);
	else
	    current_pos = next_stripe;
    }

    args.put_skip ();
    line_no++;

    if (!args.abbrev_mode || line_no < page_length) {
	to_arrows.text [to_arrows.length] = '\0';
	args.print_to_arrows (to_arrows.text);
	line_no++;
    }

    if (!args.abbrev_mode || line_no < page_length) {
	horizontal_bar.text [horizontal_bar.length] = '\0';
	args.print_horizontal_bar (horizontal_bar.text);
	line_no++;
    }
}



static void	tree_print (void)
{
    line_no = 0;

    for (start_pos = minpos; start_pos <= maxpos; start_pos = next_stripe) {
	next_stripe = start_pos + args.page_width;

	if (!args.abbrev_mode)
	    args.put_page ();

	for (line = lines; line != NULL; line = line->next) {
	    print_one_line ();
	}
    }

    if (!args.abbrev_mode)
	args.put_page ();
}



static char	*strings = {NULL};



void	sxpptree (USER_PTR tree,
		  SXINT min_dist,
		  SXINT min_dist_on_subtrees,
		  SXINT page_width,
		  SXINT (*get_key_length) (USER_PTR),
		  USER_PTR (*get_left_son) (USER_PTR),
		  USER_PTR (*get_right_brother) (USER_PTR),
		  SXINT (*get_sons_number) (USER_PTR),
		  void (*print_from_arrows) (char *),
		  void (*print_horizontal_bar) (char *),
		  void (*print_key) (USER_PTR, SXINT, SXINT),
		  void (*print_to_arrows) (char *),
		  void (*put_blanks) (SXINT),
		  void (*put_page) (void),
		  void (*put_skip) (void),
		  SXINT switches)
{
    args.min_dist = min_dist < 1 ? 3 : min_dist;
    args.min_dist_on_subtrees = min_dist_on_subtrees < args.min_dist ? args.min_dist : min_dist_on_subtrees;
    args.page_width = page_width < min_dist_on_subtrees ? (min_dist_on_subtrees < 132 ? 132 : min_dist_on_subtrees) :
	 page_width;
    args.get_key_length = get_key_length;
    args.get_left_son = get_left_son;
    args.get_right_brother = get_right_brother;
    args.get_sons_number = get_sons_number;
    args.print_from_arrows = print_from_arrows;
    args.print_horizontal_bar = print_horizontal_bar;
    args.print_key = print_key;
    args.print_to_arrows = print_to_arrows;
    args.put_blanks = put_blanks;
    args.put_page = put_page;
    args.put_skip = put_skip;
    args.abbrev_mode = (switches & (SXINT) 0x8000) != 0;

    if (args.abbrev_mode) {
	page_length = args.get_key_length (NULL);
	max_level = (page_length + 2) / 4;
    }

    if (setjmp (no_space))
	fprintf (stderr, "\npp_trees: no more space available\n");
    else {
	{
	    SXUINT strlength = args.page_width + 1;

	    strings = (char*) allocate (3 * strlength * sizeof (char));
	    to_arrows.text = (from_arrows.text = (horizontal_bar.text = strings) + strlength) + strlength;
	}

	place (tree, &lines, (SXINT)1, true);
	init_h_bar_ovfl ();

	if (args.abbrev_mode)
	    abbreviate_tree ();
	else
	    min_max_pos ();

	tree_print ();
    }

    lines_destroy (lines);
    lines = NULL;

    if (strings != NULL)
	free (strings), strings = NULL;

    deallocate_areas ();
}
