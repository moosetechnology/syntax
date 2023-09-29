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
/*** THIS FILE IS NOT PART OF THE SYNTAX LIBRARY libsx.a ***/ 

/* Genere a partir d'une Range Concatenation Grammar
   Une RCG equivalente dont le nombre de variables par arg est minimal */



#include "sxversion.h"
#include "sxunix.h"
char WHAT_RFSA_PARSER[] = "@(#)SYNTAX - $Id: RFSA_parser.c 2227 2018-09-26 16:22:59Z garavel $" WHAT_DEBUG;

static char	ME [] = "RFSA_parser";
/* A FAIRE
   - traitement des predicats externes.  Faire fsa_traversal(fsa1, p, i, fsa2) ou fsa1 sera/est le fsa courant et fsa2
     sera le fsa qu'il faudra appeler sur final state (i.e. on aura un truc du style r = fsa_traversal(fsa2, q, j, fsa1))
     Une transition p ->&A q est representee par p ->-(tmax+A) q et fait appel par (*prdct_call[A]) (i, fsa2) a la fonction
     _A_fsa (i, fsa2) (liee par l'editeur de liens) qui appelle fsa_traversal(fsa1, init_state, i, fsa2) ou fsa1 est le fsa
     de A et init_state son etat initial
*/

/* Utilisation :
   - guiding parser pour du 1-RCG simple
   - guide pour un CF parser nouvelle mode
   - tagger
   - shallow parser
*/

/* Codage de l'interprete du dfsa */
#include "lrfsa.h"
#include "sxmatrix2vector.h"

struct trans {
  SXINT red, q;
};

#include RFSA /* RFSA SERA REMPLACE GRACE A UNE OPTION DE COMPILATION */

/* Memoization */
static SXBA *already_processed, *ret_vals;

static SXBOOLEAN rfsa_process_p_i ();
static void rfsa_final ();


void
_rfsa_if ()
{
  SXINT	tok, prod;
  SXBA 	set, t_set;

  rfsaGs [rfsa_global_pid] = &G;

#if EBUG
  printf ("final_state = %i, prev_final_state = %i\n", final_state, prev_final_state);
#endif

  G.pid = rfsa_global_pid++;
  G.process_fsa = rfsa_process_p_i;
  G.final_fsa = rfsa_final;

  already_processed = sxbm_calloc (fsa_state_nb+1, n+1);
  ret_vals = sxbm_calloc (fsa_state_nb+1, n+1);

  /* Le 04/10/2002 : C'est l'appelant qui gere les autres allocations */
}



static void
rfsa_final ()
{
  /* lfsa_final peut etre appele plusieurs fois au cas ou` rfsa sert de guide */
  if (already_processed) sxbm_free (already_processed), already_processed = NULL;
  if (ret_vals) sxbm_free (ret_vals), ret_vals = NULL;
}


/* Le source est traite de droite a gauche */
/* L'appel initial depuis RCG_parser est de la forme lrfsa_process_p_i (1, 0) ou lrfsa_process_p_i (1, n) */
static SXBOOLEAN
rfsa_process_p_i (p, i)
     SXINT p, i;
{
  SXINT j, top, t, q, prod, k, bot, cur, bot2, cur2, top2, base;
  SXBA in_source_set, out_source_set, laq;
  SXBOOLEAN ret_val, is_first, done;
  struct trans *ptrans;

#if EBUG
  call_level++;
  printf ("in(%i): p=%i, i=%i\n", call_level, p, i);
#endif

  if (!SXBA_bit_is_reset_set (already_processed [p], i)) {
#if EBUG
    printf ("out_memo(%i): %s\n", call_level, SXBA_bit_is_set (ret_vals [p], i) ? "SXTRUE" : "SXFALSE");
    call_level--;
#endif
    return SXBA_bit_is_set (ret_vals [p], i);
  }

  ret_val = SXFALSE;

  /* On traite les transitions terminales sur le terminal ti+1 ou ti-1*/
  if ((base = vector_base (fsa_shift, p)) != 0) {
    /* L'automate a des transitions terminales */
    in_source_set = glbl_source [i];
    out_source_set = glbl_out_source [i];

    j = i-1;
    t = -1;

    while ((t = sxba_scan (in_source_set, t)) >= 0) {
      /* On examine tous les terminaux du source au niveau i */
      if ((q = fsa_shift_vector_get (p, t, base)) != fsa_shift_ERROR) {
#if EBUG
	printf ("\t%i -->%i %i\n", p, -t, q);
#endif
	if (t == 0) {
	  /* t == 0 => t==eof, j==0 q est final */
	  /* Comme il n'y a pas de cycles, aucune chaine de reduce ne peut marcher, on peut sortir */
	  SXBA_1_bit (ret_vals [p], i);
#if EBUG
	  printf ("out(%i): SXTRUE\n", call_level);
	  call_level--;
#endif
	  return SXTRUE;
	}
	
	if (rfsa_process_p_i (q, j)) {
	  ret_val = SXTRUE;

	  SXBA_1_bit (out_source_set, t);
	}
      }
#if EBUG
      else {
	printf ("\t%i -->%i SXFALSE\n", p, -t);
      }
#endif
    }
  }

  /* On traite les reductions de borne sup i */
  if (fsa_red_disp) {
    top = fsa_red_disp [p+1];
    bot = fsa_red_disp [p];

    if (top > bot) {
      /* Il y a des reductions */
      for (cur = bot; cur < top; cur++) {
	ptrans = fsa_red_trans + cur;
	q = ptrans->q;

	if (look_ahead == NULL || (laq = look_ahead [q]) == NULL || is_AND (glbl_source [i], laq)) {
	  /* prod est simple */
	  prod = ptrans->red; 

	  if (prod <= last_prod) {
	    /* Attention, si is_shallow, prod est un nt et G.Lex == NULL */
#if EBUG3
	    printf ("%i[%i..?]\n", prod, i); 
#endif /* EBUG3 */ 
	    if ((G.Lex == NULL || SXBA_bit_is_set (G.Lex, prod)) && rfsa_process_p_i (q, i)) {
	      ret_val = SXTRUE;

	      if (G.fsa_valid_prod_set) SXBA_1_bit (G.fsa_valid_prod_set, prod);

#if is_semantics
#if EBUG2
	      G.instantiated_prod_nb++;
#endif
	      if (prod && G.prod2lub) SXBA_1_bit (G.prod2lub [prod], i);
#endif  
	    }
	  }
	  else {
	    /* prod est multiple */
#if EBUG3
	    printf ("%i={", prod); 
	    k = prod-last_prod;
	    is_first = SXTRUE;

	    for (top2 = mtrans_disp [k+1], cur2 = mtrans_disp [k]; cur2 < top2; cur2++) {
	      k = mtrans_list [cur2];

	      if (is_first)
		is_first = SXFALSE;
	      else
		fputs (", ", stdout);

	      printf ("%i", k); 
	    }
		
	    printf ("}[%i..?]\n", i); 
#endif /* EBUG3 */

	    /* Si G.Lex != NULL on ne lance rfsa_process_p_i que s'il y a au moins une 
	       prod valide (apres essai, ca vaut le coup) */
	    if (G.Lex) {
	      prod -= last_prod;
	      top2 = mtrans_disp [prod+1];
	      bot2 = mtrans_disp [prod];
	      
	      for (cur2 = bot2; cur2 < top2; cur2++) {
		prod = mtrans_list [cur2];

		if (SXBA_bit_is_set (G.Lex, prod))
		  /* Y-en a une de bonne */
		  break;
	      }
	      
	      if (cur2 == top2)
		/* Echec de cette transition multiple, on regarde la suivante */
		continue;
	    }

	    if (rfsa_process_p_i (q, i)) {
#if is_semantics
	      if (G.Lex == NULL) {
		prod -= last_prod;
		top2 = mtrans_disp [prod+1];
		bot2 = mtrans_disp [prod];
	      }
	      
	      for (cur2 = bot2; cur2 < top2; cur2++) {
		prod = mtrans_list [cur2];

		if (G.Lex == NULL || SXBA_bit_is_set (G.Lex, prod)) {
		  ret_val = SXTRUE;

		  if (G.fsa_valid_prod_set) SXBA_1_bit (G.fsa_valid_prod_set, prod);

#if EBUG2
		  G.instantiated_prod_nb++;
#endif
		  if (prod && G.prod2lub) SXBA_1_bit (G.prod2lub [prod], i);
		}
	      }
#else /* !is_semantics */
	      if (G.fsa_valid_prod_set) {
		if (G.Lex == NULL) {
		  prod -= last_prod;
		  top2 = mtrans_disp [prod+1];
		  bot2 = mtrans_disp [prod];
		}

		for (cur2 = bot2; cur2 < top2; cur2++) {
		  prod = mtrans_list [cur2];

		  if (G.Lex == NULL || SXBA_bit_is_set (G.Lex, prod)) {
		    SXBA_1_bit (G.fsa_valid_prod_set, prod);
		  }
		}
	      }

	      ret_val = SXTRUE;
#endif /* !is_semantics */
	    }
	  }
	}
      }
    }
  }

  if (ret_val)
    SXBA_1_bit (ret_vals [p], i);

#if EBUG
    printf ("out(%i): %s\n", call_level, ret_val ? "SXTRUE" : "SXFALSE");
    call_level--;
#endif

  return ret_val;
}
