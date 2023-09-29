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





#include "sxversion.h"
#include "sxunix.h"
#include "varstr.h"

char WHAT_SXCOMMENT_MNGR[] = "@(#)SYNTAX - $Id: sxcomment_mngr.c 2428 2023-01-18 12:54:10Z garavel $" WHAT_DEBUG;



void
sxcomment_alloc (struct sxcomment *sxcomment_ptr, SXINT size)
{
  sxcomment_ptr->vstr_heads = (VARSTR *) sxalloc (sxcomment_ptr->vstr_heads_size = 1, sizeof (VARSTR));
  sxcomment_ptr->vstr_heads_top = 0;
  sxcomment_ptr->vstr_heads [0] = varstr_alloc (size+1);
}



void
sxcomment_clear (struct sxcomment *sxcomment_ptr)
{
  SXINT x;

  for (x = 0; x <= sxcomment_ptr->vstr_heads_top; x++) {
    varstr_raz (sxcomment_ptr->vstr_heads [x]);
  }

  sxcomment_ptr->vstr_heads_top = 0;
}



void
sxcomment_free (struct sxcomment *sxcomment_ptr)
{
  SXINT x;

  for (x = 0; x <= sxcomment_ptr->vstr_heads_top; x++) {
    varstr_free (sxcomment_ptr->vstr_heads [x]);
  }

  sxfree (sxcomment_ptr->vstr_heads), sxcomment_ptr->vstr_heads = NULL;
}

/* Reserve ds sxcomment_ptr une place suffisante pour ranger une chaine de taille string_lgth
   et finalise la structure le NUL en string_lgth+1 est mis */
char *
sxcomment_book (struct sxcomment *sxcomment_ptr, SXUINT string_lgth)
{
  SXINT  length;
  SXUINT size;
  VARSTR vstr;

  vstr = sxcomment_ptr->vstr_heads [sxcomment_ptr->vstr_heads_top];

  if (string_lgth >= (SXUINT) (vstr->last - vstr->current)) {
    /* ne tient pas */
    length = 0;

    if (++sxcomment_ptr->vstr_heads_top == sxcomment_ptr->vstr_heads_size) {
      sxcomment_ptr->vstr_heads = (VARSTR *) sxrecalloc (sxcomment_ptr->vstr_heads, sxcomment_ptr->vstr_heads_size, 
							 2*sxcomment_ptr->vstr_heads_size, sizeof (VARSTR));
      sxcomment_ptr->vstr_heads_size *= 2;
    }

    size = (SXUINT) varstr_maxlength (vstr);
    vstr = sxcomment_ptr->vstr_heads [sxcomment_ptr->vstr_heads_top];

    if (vstr == NULL) {
      size *= 2;

      if (size <= string_lgth)
	size = string_lgth+1;

      vstr = sxcomment_ptr->vstr_heads [sxcomment_ptr->vstr_heads_top] = varstr_alloc ((SXINT) (size+1));
    }
    else {
      /* Il y a reallocation si ca ne tient pas (c'est le premier commentaire qu'on veut mettre dans
	 ce vstr, aucun pointeur entrant n'existe encore) */
      while (string_lgth >= (SXUINT) varstr_maxlength (vstr))
	vstr = varstr_realloc (vstr);

      sxcomment_ptr->vstr_heads [sxcomment_ptr->vstr_heads_top] = vstr;
    }
  }
  else {
    length = varstr_length (vstr);
  }

  /* On finalise vstr */
  *(vstr->current += string_lgth) = SXNUL;
  /* ... et on pointe derriere la chaine que l'on va ranger. */
  vstr->current++;
  /* On a vstr->current <= vstr->last
     Le coup d'apres, meme si on range une chaine de taille nulle, il y aura debordement */

  return vstr->first + length;
}

void
sxcomment_mngr (SXINT sxcomment_mngr_what, SXINT size)
{
  switch (sxcomment_mngr_what) {
  case SXOPEN:
    sxcomment_alloc (&(sxplocals.sxcomment), size);
    break;

  case SXINIT:
    break;

  case SXFINAL:
    break;

  case SXCLOSE:
    sxcomment_free (&(sxplocals.sxcomment));
    break;

  default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
    sxtrap("sxcomment_mngr","unknown switch case #1");
#endif
    break;
  }
}

