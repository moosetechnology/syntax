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
#ifndef lrfsa_h
#define lrfsa_h

/* lrfsa.h */

extern bool          is_AND();

typedef void 		(*vfun)();
typedef bool		(*bfun)();
typedef SXINT		(*ifun)();

struct fsaG {
  SXINT     prod_nb, nt_nb, init_state, pid, instantiated_prod_nb;
  SXBA    *t2prod_set;
  SXBA    Lex, fsa_valid_prod_set, *prod2lub;
  bfun    process_fsa;
  vfun    final_fsa;
  bool shallow;
};

SXINT                    lfsa_global_pid, rfsa_global_pid;

extern struct fsaG     *lfsaGs [], *rfsaGs [];

#define fsa_prod2lb(c,b) (G.prod2lbs==NULL || SXBA_bit_is_set(G.prod2lbs[c],b))
#define fsa_prod2ub(c,b) (G.prod2ubs==NULL || SXBA_bit_is_set(G.prod2ubs[c],b))
#define fsa_nt2lb(c,b) (G.nt2lbs==NULL || SXBA_bit_is_set(G.nt2lbs[c],b))
#define fsa_nt2ub(c,b) (G.nt2ubs==NULL || SXBA_bit_is_set(G.nt2ubs[c],b))


SXINT                    n;

#if is_sdag
SXBA	               *glbl_source, *glbl_out_source; 
#else
SXINT	               *glbl_source, *glbl_out_source; /* A FAIRE */
#endif

#endif /* #ifndef lrfsa_h */
