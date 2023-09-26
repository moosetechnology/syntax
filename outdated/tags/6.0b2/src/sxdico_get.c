/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1999 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *  Produit de l'equipe ATOLL.			  *
   *                                                      *
   ******************************************************** */


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030505 11:05 (phd):	Modif pour calmer le compilo SGI	*/
/************************************************************************/
/* Jeu 30 Sep 1999 11:29 (pb):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/


#include "sxcommon.h"
#include "sxdico.h"

char WHAT_SXDICO_GET[] = "@(#)SYNTAX - $Id: sxdico_get.c 527 2007-05-09 14:02:47Z rlacroix $" WHAT_DEBUG;

extern void sxtrap (char *caller, char *message);

/* Recherche kw ds le dico et retourne le code de kw ou 0 si kw n'existe pas */
/* Si dico->partiel, (kw, kwl) indique la partie infixe non reconnue. */
int
sxdico_get (dico, kw, kwl)
    struct   dico	*dico;
    unsigned char	**kw;
    int			*kwl;
{
  unsigned int	is_suffix, base, tooth = dico->init_base, suffix_mask = dico->is_suffix_mask,
                base_shift = dico->base_shift, class_mask = dico->class_mask, stop_mask = dico->stop_mask,
                *comb_vector = dico->comb_vector;
  unsigned char	class, *pbs, *char2class = dico->char2class;
  int		lgth = *kwl;

  if (tooth == 0 || lgth < 0)
    return 0;

  base_shift = dico->base_shift;

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

      return ((int) tooth) >> base_shift;
    }

    if ((class = char2class [is_suffix ? *pbs-- : *((*kw)++)]) == 0)
      break;

    lgth = --*kwl;
    tooth = comb_vector [base+class];

    if ((tooth & class_mask) != class)
      break;

    if (tooth & stop_mask) {
      if (lgth == 0 || dico->partiel)
	return ((int) tooth) >> base_shift;

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
int
sxdico_get_bounded (dico, kw, kwl, kind_bound)
    struct   dico	*dico;
    unsigned char	**kw;
    int			*kwl, kind_bound;
{
  unsigned int	is_suffix, base, tooth = dico->init_base, suffix_mask = dico->is_suffix_mask,
                base_shift = dico->base_shift, class_mask = dico->class_mask, stop_mask = dico->stop_mask,
                *comb_vector = dico->comb_vector;
  unsigned char	class, *pbs, *char2class = dico->char2class, *store_kw;
  int		lgth = *kwl;
  int           is_final_state, state_tooth, id, prev_id, store_kwl;

#if EBUG
  unsigned  int ok_kind;
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
      id = ((int) state_tooth) >> base_shift;

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

#if 0
    /* PB : mais qu'a donc fait BS ici !!?? */
    if (lgth == *kwl) {
      /* BS: mystère; fait boucler; break (exemple: ombellíforme (noter l'accent sur le i))*/
      /* solution non satisfaisante, car on a trouvé qqch qd même!!! */
      id = 0;
      break;
    }
#endif /* 0 */

    lgth = --*kwl;

    if (tooth & stop_mask) {
      /* On vient de trouver un prefixe/suffixe qui n'est pas lui-meme un prefixe/suffixe d'un autre mot du dico */
      /* => on le retourne */
      id = ((int) tooth) >> base_shift;
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

