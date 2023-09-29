/* ********************************************************
   *                                                      *
   *                                                      *
   * Copyright (c) 1986 by Institut National de Recherche *
   *                    en Informatique et en Automatique *
   *                                                      *
   *                                                      *
   ******************************************************** */




/* ********************************************************
   *                                                      *
   *     Produit de l'equipe Langages et Traducteurs.     *
   *                                                      *
   ******************************************************** */

#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
char WHAT_LECLAGFREE[] = "@(#)SYNTAX - $Id: lecl_ag_free.c 529 2007-05-09 14:26:33Z rlacroix $" WHAT_DEBUG;

extern VOID lecl_gen_free ();



VOID	lecl_ag_free ()
{
    register int	i;

    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = TRUE;
	}

	fputs ("Freeing\n", sxtty);
    }

    lecl_gen_free ();

    if (action_or_prdct_code)
	sxfree (action_or_prdct_code);

    if (transition_matrix) {
	for (i = last_state_no; i >= 1; i--)
	    sxfree (transition_matrix [i]);

	sxfree (transition_matrix);
    }

    if (new_sc_to_char_set)
	sxbm_free (new_sc_to_char_set);

    if (compound_classes)
	sxbm_free (compound_classes);

    if (ctxt)
	sxbm_free (ctxt);

    if (is_re_generated)
	sxfree (is_re_generated);

    if (esc_to_sc_prdct)
	sxfree (esc_to_sc_prdct);

    if (sc_prdct_to_esc) {
	for (i = smax; i >= 1; i--)
	    sxfree (sc_prdct_to_esc [i]);

	sxfree (sc_prdct_to_esc);
    }

    if (sc_to_char_set)
	sxbm_free (sc_to_char_set);

    if (class_set)
	sxfree (class_set);

    if (kod)
	sxbm_free (kod);

    if (tnext)
	sxbm_free (tnext);

    if (tfirst)
	sxbm_free (tfirst);

    if (suivant)
	sxbm_free (suivant);

    if (ers)
	sxfree (ers);

    if (synonym)
	sxfree (synonym);

    if (synonym_list)
	sxfree (synonym_list);

    if (target_collate)
	sxfree (target_collate);

    if (target_code)
	sxfree (target_code);

    if (simple_classe)
	sxfree (simple_classe);

    if (classe)
	sxfree (classe);

    if (t_is_defined_by_a_token)
	sxfree (t_is_defined_by_a_token);

    if (is_a_keyword)
	sxfree (is_a_keyword);

    if (exclude)
	sxfree (exclude);

    if (keyword)
	sxfree (keyword);

    if (keywords)
	sxfree (keywords);

    if (RE_to_T)
	sxbm_free (RE_to_T);

    if (ers_disp)
	sxfree (ers_disp);

    if (not_declared)
	sxfree (not_declared);

    if (is_class_used_in_re)
	sxfree (is_class_used_in_re);

    if (class_to_char_set)
	sxbm_free (class_to_char_set);

    if (prdct_to_ers)
	sxfree (prdct_to_ers);

    if (action_or_prdct_to_ate) {
	action_or_prdct_to_ate -= action_size;
	sxfree (action_or_prdct_to_ate);
    }

    if (abbrev_or_class_to_ate) {
	abbrev_or_class_to_ate -= abbrev_size;
	sxfree (abbrev_or_class_to_ate);
    }

    if (abbrev) {
	abbrev -= abbrev_size;
	sxfree (abbrev);
    }

    if (ate_to_t)
	sxfree (ate_to_t);

    if (t_to_ate)
	sxfree (t_to_ate);

    if (ate_to_token_no)
	sxfree (ate_to_token_no);

    if (fsa_trans)
	sxfree (fsa_trans);

    if (item)
	sxfree (item);

    if (fsa)
	sxfree (fsa);

    if (state)
	sxfree (state);

    if (substate)
	sxfree (substate);

    if (follow)
	sxbm_free (follow);

    if (is_a_generic_terminal)
	sxfree (is_a_generic_terminal); /* end lecl_ag_free */
}
