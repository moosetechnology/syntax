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
#ifndef sxstack_h
#define sxstack_h

/* Cas des piles "dynamiques" dont la taille peut varier */
#define DALLOC_STACK(t,l)  t = (SXINT*) sxalloc (l+2, sizeof (SXINT))+1, t [-1] = l, t [0] = 0
#define DFREE_STACK(t)     sxfree (t-1), t = NULL 
#define DTOP(t)		  (*(t))
#define DSIZE(t)	  ((t)[-1])
#define DPOP(t)		  (t)[DTOP(t)--]
#define DPUSH(t,x)	  (((++DTOP(t) >= DSIZE(t)) ? (DSIZE(t)*=2, t=(SXINT*) sxrealloc ((t)-1, (DSIZE(t))+2, sizeof (SXINT))+1) : (0)), (t)[DTOP(t)]=x)
/* Secure PUSH */
#define DSPUSH(t,x)	  ((t)[++DTOP(t)]=x)
/* before SPUSHes */
#define DCHECK(t,n)	  (((DTOP(t)+(n) >= DSIZE(t)) ? (DSIZE(t)=DSIZE(t)*2+(n), t=(SXINT*) sxrealloc ((t)-1, DSIZE(t)+2, sizeof (SXINT))+1) : (0)))
#define DRAZ(t)		  *t=0

/* Cas des piles statiques */
#define PUSH(s,x)	  (s)[++*(s)]=(x)
#define POP(s)		  (s)[(*(s))--]
#define IS_EMPTY(s)	  (*(s)==0)
#define TOP(s)		  (*(s))
#define RAZ(s)		  *(s)=0

#if 0
/* Package de manipulation de file FIFO d'entiers (on pourra mettre c,a ds la librairie s'il y a des utilisations) */
struct FIFO {
  SXINT bot, top, size;
  SXINT *bot_ptr /* pointe vers le 1er element de la structure */
      , *top_ptr /* pointe derriere le dernier element de la structure */
      , *left_ptr /* pointe vers le dernier element pope' */
      , *right_ptr /* pointe vers le dernier element pushe' */
    ;
};

static void
fifo_alloc (struct FIFO *fifo_ptr, SXINT size)
{
  fifo_ptr->bot_ptr = fifo_ptr->left_ptr = fifo_ptr->right_ptr = (SXINT*) sxalloc (size, sizeof (SXINT));
  fifo_ptr->top_ptr = fifo_ptr->bot_ptr+size;
}


static void
fifo_free (struct FIFO *fifo_ptr)
{
  sxfree (fifo_ptr->bot), fifo_ptr->bot = NULL;
}

static void
fifo_oflw (struct FIFO *fifo_ptr)
{ /* On a fifo_ptr->right_ptr == fifo_ptr->left_ptr */
  SXINT old_size, border;
  SXINT *old_cur_ptr, *cur_ptr;

  old_size = (fifo_ptr->top_ptr - fifo_ptr->bot_ptr);
  border = fifo_ptr->left_ptr - fifo_ptr->bot_ptr;
  
  fifo_ptr->bot_ptr = (SXINT*) sxrealloc (fifo_ptr->bot_ptr, 2*old_size, sizeof (SXINT));
  fifo_ptr->right_ptr = fifo_ptr->bot_ptr+border;
  fifo_ptr->top_ptr = fifo_ptr->bot_ptr+2*old_size;
  fifo_ptr->left_ptr = fifo_ptr->top_ptr+border - (old_size-border);

  /* Il faut deplacer la "partie basse" */
  old_cur_ptr = fifo_ptr->bot_ptr+border;

  for (cur_ptr = fifo_ptr->left_ptr+1; cur_ptr < fifo_ptr->top_ptr; cur_ptr++)
    *++old_cur_ptr = *cur_ptr;
}

static SXBOOLEAN
fifo_is_empty (struct FIFO *fifo_ptr)
{
  return (fifo_ptr->left_ptr == fifo_ptr->right_ptr);
}

static void
fifo_clear (struct FIFO *fifo_ptr)
{
  fifo_ptr->left_ptr = fifo_ptr->right_ptr = fifo_ptr->bot_ptr;
}

static SXBOOLEAN
fifo_pop (struct FIFO *fifo_ptr, SXINT *elem_ptr)
{
  if (fifo_ptr->left_ptr == fifo_ptr->right_ptr)
    return SXFALSE; /* vide */

  *elem_ptr = *fifo_ptr->right_ptr;

  if (fifo_ptr->right_ptr == fifo_ptr->bot_ptr)
    fifo_ptr->right_ptr = fifo_ptr->top_ptr+1;
  else
    fifo_ptr->right_ptr--;

  return SXTRUE;
}

static void
fifo_push (struct FIFO *fifo_ptr, SXINT elem)
{
  if (++fifo_ptr->right_ptr >= fifo_ptr->top_ptr)
    /* On reboucle */
    fifo_ptr->right_ptr = fifo_ptr->bot_ptr;

  if (fifo_ptr->right_ptr == fifo_ptr->left_ptr)
    /* Ca deborde */
    fifo_oflw (fifo_ptr);
    
  *(fifo_ptr->right_ptr) = elem;
}
#endif /* 0 */


#endif /* sxstack_h */
