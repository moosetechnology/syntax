/* ********************************************************
   *							  *
   *							  *
   * Copyright (c) 1987 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *							  *
   *							  *
   ******************************************************** */




/* ********************************************************
   *							  *
   *  Produit de l'equipe Langages et Traducteurs.  (pb)  *
   *							  *
   ******************************************************** */

/* 
   sxba_2op : lhs op1 rhs1 op2 rhs2
   sxba_incr: lhs = {i+incr | i \in rhs}
   sxba_decr: lhs = {i-decr | i \in rhs}
   sxba_or_with_test: lhs U= rhs + retourne vrai ssi la lhs a change'
*/


/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 02-10-06 16:50 (pb):		Creation				*/
/************************************************************************/


#include        "sxcommon.h"
#include "sxba.h"

char WHAT_SXBA_2OP[] = "@(#)SYNTAX - $Id: sxba_2op.c 807 2007-10-08 14:37:01Z rlacroix $" WHAT_DEBUG;

extern void sxperror (char *);
extern void sxtrap (char *caller, char *message);

/* Realise lhs op1= rhs1 op2 rhs2 */
/* Retourne TRUE ssi lhs est non vide */
/* Attention, on peut avoir lhs == rhs1 */
BOOLEAN
sxba_2op (SXBA lhs, SXINT op1, SXBA rhs1, SXINT op2, SXBA rhs2)
{
  SXBA	   lhs_bits_ptr, rhs_bits_ptr1, rhs_bits_ptr2;
  SXBA_ELT slices_number = NBLONGS (BASIZE (rhs2));
  SXBA_ELT result = (SXBA_ELT)0, operand2;

  sxinitialise(lhs_bits_ptr); /* pour faire taire "gcc -Wuninitialized" */
  sxinitialise(rhs_bits_ptr1); /* pour faire taire "gcc -Wuninitialized" */
#if EBUG
    sxbassert (rhs2 != NULL, "2op (rhs2 == NULL)");
    if (rhs1) {sxbassert (*rhs1 == *rhs2, "2op (|rhs1|!=|rhs2|)");}
    if (lhs) {sxbassert (*lhs == *rhs2, "2op (|lhs|!=|rhs2|)");}
    if (lhs) {sxbassert ((*(lhs+NBLONGS (BASIZE (lhs))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs) - 1))) == 0, "2op");}
    if (rhs1) {sxbassert ((*(rhs1+NBLONGS (BASIZE (rhs1))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs1) - 1))) == 0, "2op");}
    sxbassert ((*(rhs2+NBLONGS (BASIZE (rhs2))) & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs2) - 1))) == 0, "2op");
#endif /* EBUG */

    if (lhs) lhs_bits_ptr = lhs + slices_number;
    if (rhs1) rhs_bits_ptr1 = rhs1 + slices_number;
    rhs_bits_ptr2 = rhs2 + slices_number;

  while (slices_number-- > 0) {
    operand2 = *rhs_bits_ptr2--;

    switch (op2) {
    case SXBA_OP_AND:
#if EBUG
      sxbassert (rhs1 != NULL, "2op (rhs1 == NULL)");
#endif /* EBUG */
      operand2 &= *rhs_bits_ptr1--;
      break;

    case SXBA_OP_OR:
#if EBUG
      sxbassert (rhs1 != NULL, "2op (rhs1 == NULL)");
#endif /* EBUG */
      operand2 |= *rhs_bits_ptr1--;
      break;

    case SXBA_OP_XOR:
#if EBUG
      sxbassert (rhs1 != NULL, "2op (rhs1 == NULL)");
#endif /* EBUG */
      operand2 ^= *rhs_bits_ptr1--;
      break;

    case SXBA_OP_MINUS:
#if EBUG
      sxbassert (rhs1 != NULL, "2op (rhs1 == NULL)");
#endif /* EBUG */
      operand2 = *rhs_bits_ptr1-- & (~operand2);
      break;

    case SXBA_OP_NOT:
      operand2 = ~operand2;
      break;

#if EBUG
    case SXBA_OP_NULL:
    case SXBA_OP_COPY:
    case SXBA_OP_IS:
      sxbassert (FALSE, "2op (bad op2)");
      break;
#endif /* EBUG */
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxba_2op","unknown switch case #1");
#endif
      break;
    }

    switch (op1) {
#if EBUG
    case SXBA_OP_NULL:
      sxbassert (lhs == NULL, "2op (lhs != NULL)");
#endif /* EBUG */

    case SXBA_OP_AND:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- &= operand2);
      break;

    case SXBA_OP_OR:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- |= operand2);
      break;

    case SXBA_OP_XOR:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- ^= operand2);
      break;

    case SXBA_OP_MINUS:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- &= ~operand2);
      break;
    case SXBA_OP_NOT:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- = ~operand2);

    case SXBA_OP_COPY:
#if EBUG
      sxbassert (lhs != NULL, "2op (lhs == NULL)");
#endif /* EBUG */
      operand2 = (*lhs_bits_ptr-- = operand2);
      break;

    case SXBA_OP_IS:
      /* Rien  */
#if EBUG
      sxbassert (lhs == NULL, "2op (lhs != NULL)");
#endif /* EBUG */
      break;
    default: /* pour faire taire gcc -Wswitch-default */
#if EBUG
      sxtrap("sxba_2op","unknown switch case #2");
#endif
      break;
    }

    result |= operand2;
  }

  return result != 0;
}

/* lhs = {i+incr | i \in rhs} 
   On peut avoir lhs == rhs
*/
BOOLEAN
sxba_incr (SXBA lhs, SXBA rhs, SXBA_INDEX incr)
{
  SXBA	                source_bits_ptr, object_bits_ptr;
  SXBA_INDEX    	delta = DIV (incr); /* decalage en mots */
  SXBA_INDEX            size = sxba_cast (BASIZE (lhs));
  SXBA_ELT              slices_number = NBLONGS (size);
  SXBA_INDEX		left_shift = MOD (incr);
  SXBA_INDEX		right_shift = SXBITS_PER_LONG - left_shift;
  SXBA_ELT		source, prev_source, result = (SXBA_ELT)0;

#if EBUG
  sxbassert (*lhs == *rhs, "sxba_incr (|X|!=|Y|)");
  sxbassert (sxba_1_rlscan (rhs, size+1) + incr < size, "sxba_incr (|X|+incr>|X|");
  sxbassert ((*(lhs+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (size-1))) == 0, "sxba_incr");
  sxbassert ((*(rhs+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (size-1))) == 0, "sxba_incr");
#endif
       
  object_bits_ptr = lhs + slices_number;
  source_bits_ptr = rhs + slices_number - delta; 

  source = *source_bits_ptr--;
  
  while (source_bits_ptr >= rhs) {
    prev_source = source;
    source = *source_bits_ptr--;
      
    /* On met :
       la partie basse de prev_source ds la partie haute de objet
       et la partie haute de source ds la partie basse de objet  */
    result |= (*object_bits_ptr-- = (prev_source << left_shift) | (source >> right_shift));
  }

  /* ... et la partie basse de source ds la partie haute de objet */
  result |= (*object_bits_ptr-- = (source << left_shift));

  while (object_bits_ptr >= lhs)
    *object_bits_ptr-- = (SXBA_ELT)0;

  return result != 0;
}


/* lhs = {i-decr | i \in rhs} 
   On peut avoir lhs == rhs
*/
BOOLEAN
sxba_decr (SXBA lhs, SXBA rhs, SXBA_INDEX decr)
{
  SXBA	                source_bits_ptr, object_bits_ptr;
  SXBA_INDEX		delta = DIV (decr); /* decalage en mots */
  SXBA_ELT              size = BASIZE (lhs);
  SXBA_ELT              slices_number = NBLONGS (size);
  SXBA_INDEX		right_shift = MOD (decr);
  SXBA_INDEX		left_shift = SXBITS_PER_LONG - right_shift;
  SXBA_ELT		source, prev_source, result = (SXBA_ELT)0;

#if EBUG
  sxbassert (*lhs == *rhs, "sxba_decr (|X|!=|Y|)");
  sxbassert (sxba_scan (rhs, -1) - decr >= 0, "sxba_decr (|X|-decr>|X|");
  sxbassert ((*(lhs+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (size-1))) == 0, "sxba_decr");
  sxbassert ((*(rhs+slices_number) & ((~((SXBA_ELT)0)) << 1 << MOD (size-1))) == 0, "sxba_decr");
#endif
       
  object_bits_ptr = lhs + slices_number - delta;
  source_bits_ptr = rhs + slices_number;

  source = *source_bits_ptr--;
  /* la partie haute de source ds la partie basse de objet  */
  result |= (*object_bits_ptr-- = (source >> right_shift));
  
  while (object_bits_ptr >= lhs) {
    prev_source = source;
    source = *source_bits_ptr--;
      
    /* On met :
       la partie basse de prev_source ds la partie haute de objet
       et la partie haute de source ds la partie basse de objet  */
    result |= (*object_bits_ptr-- = (prev_source << left_shift) | (source >> right_shift));
  }
  
  if (delta) {
    object_bits_ptr = lhs + slices_number;

    do {
      *object_bits_ptr-- = (SXBA_ELT)0;
    } while (--delta > 0);
  }

  return result != 0;
}


/* Analogue a sxba_or mais retourne vrai ssi la lhs change */
BOOLEAN
sxba_or_with_test (SXBA lhs_bits_array, SXBA rhs_bits_array)
{
  SXBA	              lhs_bits_ptr, rhs_bits_ptr;
  SXBA_ELT            slices_number = NBLONGS (BASIZE (rhs_bits_array));
  SXBA_ELT            lwork, work;
  BOOLEAN             ret_val = FALSE;
    
  lhs_bits_ptr = lhs_bits_array + slices_number, rhs_bits_ptr = rhs_bits_array + slices_number;

#if EBUG
  sxbassert (*lhs_bits_array == *rhs_bits_array, "sxba_or_with_test (|X|!=|Y|)");
  sxbassert ((*lhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (lhs_bits_array) - 1))) == 0, "sxba_or_with_test");
  sxbassert ((*rhs_bits_ptr & ((~((SXBA_ELT)0)) << 1 << MOD (BASIZE (rhs_bits_array) - 1))) == 0, "sxba_or_with_test");
#endif

  while (slices_number-- > 0) {
    work = (*rhs_bits_ptr-- | (lwork = *lhs_bits_ptr));

    if (lwork /* ancienne valeur */ != work /* nouvelle valeur */) {
      /* A change' */
      ret_val = TRUE;
      *lhs_bits_ptr = work;
    }

    lhs_bits_ptr--;
  }

  return ret_val;
}
