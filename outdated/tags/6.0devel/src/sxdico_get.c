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



#include "sxversion.h"
#include "sxcommon.h"
#include "sxdico.h"

char WHAT_SXDICO_GET[] = "@(#)SYNTAX - $Id: sxdico_get.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);

/* Recherche kw ds le dico et retourne le code de kw ou 0 si kw n'existe pas */
/* Si dico->partiel, (kw, kwl) indique la partie infixe non reconnue. */
SXBA_INDEX
sxdico_get (struct dico *dico, unsigned char **kw, SXINT *kwl)
{
  SXBA_INDEX    is_suffix, base, tooth = dico->init_base;
  SXUINT        base_shift = dico->base_shift;
  SXUINT        suffix_mask = dico->is_suffix_mask,
                class_mask = dico->class_mask, stop_mask = dico->stop_mask;
  SXBA_INDEX    *comb_vector = dico->comb_vector;
  unsigned char	class, *pbs, *char2class = dico->char2class;
  SXINT		lgth = *kwl;

  if (/* tooth == 0 || modif du 07/12/07 (pour nelle version) */ lgth < 0)
    return 0;

  /* base_shift = dico->base_shift; deja fait !! */

  base = tooth >> base_shift;

  if (dico->max == 0)
    /* dico vide ne specifie que la chaine vide */
    return lgth == 0 ? base : 0;

  is_suffix = tooth & suffix_mask;
  pbs = *kw + lgth - 1;

  for (;;) {
    if (lgth == 0) {
      tooth = comb_vector [base];

      if ((tooth & class_mask) != 0 || (tooth & stop_mask) == 0)
	break;

      return (tooth) >> base_shift;
    }

    if ((class = char2class [is_suffix ? *pbs-- : *((*kw)++)]) == 0)
      break;

    lgth = --*kwl;
    tooth = comb_vector [base+class];

    if ((tooth & class_mask) != class)
      break;

    if (tooth & stop_mask) {
      if (lgth == 0 || dico->partiel)
	return (tooth) >> base_shift;

      break;
    }

    is_suffix = tooth & suffix_mask;
    base = tooth >> base_shift;
  }

  return 0;
}

#ifndef EBUG
#define EBUG 1
#endif
/* Recherche ds le dico le plus grand prefixe ou suffixe (kind_bound == PREFIX_BOUND ou kind_bound == SUFFIX_BOUND)
   p de kw et retourne le code de p ou 0 si p n'existe pas */
/* Le dico doit etre construit avec l'option PREFIX_BOUND (ou SUFFIX_BOUND resp.) (donc is_suffix doit toujours etre faux) */
/* (kw, kwl) indique la partie suffixe de kw non reconnue. */
SXBA_INDEX
sxdico_get_bounded (struct dico *dico, unsigned char **kw, SXINT *kwl, SXINT kind_bound)
{
  SXBA_INDEX    is_suffix, base, tooth = dico->init_base;
  SXUINT  base_shift = dico->base_shift;
  SXUINT suffix_mask = dico->is_suffix_mask,
                class_mask = dico->class_mask, stop_mask = dico->stop_mask;
  SXBA_INDEX    *comb_vector = dico->comb_vector;
  unsigned char	class, *pbs, *char2class = dico->char2class, *store_kw;
  SXINT		lgth = *kwl;
  SXINT           is_final_state;
  SXBA_INDEX    state_tooth, id, prev_id;
  SXINT           store_kwl;

#if EBUG
  SXUINT ok_kind;
#endif

  sxinitialise(store_kwl); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(store_kw); /* pour faire taire "gcc -Wuninitialized" */
  if (tooth == 0 || lgth < 0)
    return 0;

  prev_id = 0;
  base_shift = dico->base_shift;

  base = tooth >> base_shift;

  if (dico->max == 0)
    /* dico vide ne specifie que la chaine vide */
    return lgth == 0 ? base : 0;

  is_suffix = tooth & suffix_mask;

#if EBUG
  ok_kind = ((is_suffix != 0) && (kind_bound == SUFFIX_BOUND)) || ((is_suffix == 0) && (kind_bound == PREFIX_BOUND));

  if (dico->partiel)
    sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-total\"");

  if (!ok_kind && kind_bound == SUFFIX_BOUND)
    sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-suffix_bound\"");

  if (!ok_kind && kind_bound == PREFIX_BOUND)
    sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-prefix_bound\"");
#endif /* EBUG */

  pbs = *kw + lgth - 1;

  for (;;) {
    if ((is_final_state = (((state_tooth = comb_vector [base]) & (class_mask+stop_mask)) == stop_mask))) {
      /* On vient de trouver un prefixe/suffixe correct de code id ... */
      id = ( state_tooth) >> base_shift;

      if (lgth == 0)
	/* ... et c'est le +long */
	break;

      /* ... mais ce n'est peut-etre pas le +long, on le note quand
	 meme au cas ou le chemin qu'on va prendre conduit a une impasse ... */
      prev_id = id;
      store_kwl = lgth;

      if (kind_bound == PREFIX_BOUND)
	store_kw = *kw;
      /* Si SUFFIX_BOUND, on ne fait rien (kw pointe tj en debut de mot) */
    }

    if (lgth == 0) {
      /* ... et !is_final_state => echec local */
      id = 0;
      break;
    }

    if ((class = char2class [is_suffix ? *pbs-- : *((*kw)++)]) == 0) {
      /* caractere inconnu */
      /* ... echec local */
      id = 0;
      break;
    }

    /* On examine la transition ... */
    tooth = comb_vector [base+class];

    if ((tooth & class_mask) != class) {
      /* Pas de transition sur le caractere courant */
      /* ... echec local */
      id = 0;
      break;
    }

    lgth = --*kwl;

    if (tooth & stop_mask) {
      /* On vient de trouver un prefixe/suffixe qui n'est pas lui-meme un prefixe/suffixe d'un autre mot du dico */
      /* => on le retourne */
      id = (tooth) >> base_shift;
      break;
    }

    is_suffix = tooth & suffix_mask;

#if EBUG
    ok_kind = ((is_suffix != 0) && (kind_bound == SUFFIX_BOUND)) || ((is_suffix == 0) && (kind_bound == PREFIX_BOUND));

    if (dico->partiel)
      sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-total\"");

    if (!ok_kind && kind_bound == SUFFIX_BOUND)
      sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-suffix_bound\"");

    if (!ok_kind && kind_bound == PREFIX_BOUND)
      sxtrap ("sxdico_get_prefix", "Rebuilt dictionary with the option \"-prefix_bound\"");
#endif /* EBUG */

    /* On fait la transition ... */
    base = tooth >> base_shift;
  }

  if (id)
    return id;

  if (prev_id) {
    /* On restaure ... */
    *kwl = store_kwl;
      
    if (kind_bound == PREFIX_BOUND)
      *kw = store_kw;
  }

  return prev_id;
}

