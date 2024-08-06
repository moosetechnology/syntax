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

#endif /* sxstack_h */
