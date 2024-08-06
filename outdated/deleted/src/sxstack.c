
/* Le fragment de code ci-dessous etait dans trunk/incl/sxstack.h */

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

static bool
fifo_is_empty (struct FIFO *fifo_ptr)
{
  return (fifo_ptr->left_ptr == fifo_ptr->right_ptr);
}

static void
fifo_clear (struct FIFO *fifo_ptr)
{
  fifo_ptr->left_ptr = fifo_ptr->right_ptr = fifo_ptr->bot_ptr;
}

static bool
fifo_pop (struct FIFO *fifo_ptr, SXINT *elem_ptr)
{
  if (fifo_ptr->left_ptr == fifo_ptr->right_ptr)
    return false; /* vide */

  *elem_ptr = *fifo_ptr->right_ptr;

  if (fifo_ptr->right_ptr == fifo_ptr->bot_ptr)
    fifo_ptr->right_ptr = fifo_ptr->top_ptr+1;
  else
    fifo_ptr->right_ptr--;

  return true;
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

