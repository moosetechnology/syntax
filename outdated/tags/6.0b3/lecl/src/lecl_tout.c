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

/*************************************************************/
/*                                                           */
/*                                                           */
/*  This program has been translated from lecl_tbls_out.pl1  */
/*  on Monday the twenty-eigth of April, 1986, at 10:05:54,  */
/*  on the Multics system at INRIA,                          */
/*  by the release 3.3g PL1_C translator of INRIA,           */
/*         developped by the "Langages et Traducteurs" team, */
/*         using the SYNTAX (*), FNC and Paradis systems.    */
/*                                                           */
/*                                                           */
/*************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                       */
/*************************************************************/



#include "sxunix.h"
#include "varstr.h"
#include "lecl_ag.h"
#include "S_tables.h"
char WHAT_LECLTOUT[] = "@(#)SYNTAX - $Id: lecl_tout.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;

/* */

extern BOOLEAN	lecl_write (struct lecl_tables_s *lecl_tables_ptr, char *langname);
extern VOID	lecl_free (struct lecl_tables_s *lecl_tables_ptr);



VOID	lecl_tbls_out (struct lecl_tables_s *lecl_tables_ptr)
{
    SXINT	i, x;

    if (sxverbosep) {
	if (!sxttycol1p) {
	    fputc ('\n', sxtty);
	    sxttycol1p = TRUE;
	}

	fputs ("Tables Output\n", sxtty);
    }

    lecl_tables_ptr->S_tbl_size.S_termax = termax;
    lecl_tables_ptr->S_tbl_size.S_last_simple_class_no = Smax;
    lecl_tables_ptr->S_tbl_size.S_xprod = xprod;
    lecl_tables_ptr->S_tbl_size.S_last_char_code = target_collate_length - 1;
    lecl_tables_ptr->S_tbl_size.S_last_state_no = last_state_no;
    lecl_tables_ptr->S_tbl_size.S_constructor_kind = 1;
    lecl_tables_ptr->S_tbl_size.S_are_comments_erased = (are_comments_defined) ? are_comments_erased : TRUE;
    lecl_tables_ptr->S_tbl_size.S_last_static_ste = last_static_ste;
    lecl_tables_ptr->S_tbl_size.S_is_user_action_or_prdct = FALSE;
    lecl_tables_ptr->S_tbl_size.S_is_non_deterministic = has_nondeterminism;
    lecl_tables_ptr->S_tbl_size.S_counters_size = counters_size;
    lecl_tables_ptr->S_tbl_size.S_check_kw_lgth = check_kw == NULL ? 0 : varstr_length (check_kw);

    for (i = 1; (i <= xprod) & !lecl_tables_ptr->S_tbl_size.S_is_user_action_or_prdct; i++)
	if (!(action_or_prdct_code [i].is_system))
	    lecl_tables_ptr->S_tbl_size.S_is_user_action_or_prdct = TRUE;

    lecl_tables_ptr->S_char_to_simple_class = (unsigned char*) sxalloc ((SXINT) (target_collate_length), sizeof (unsigned
	 char));

    lecl_tables_ptr->S_check_kw = check_kw == NULL ? NULL : varstr_tostr (check_kw);

    for (i = 0; i < target_collate_length; i++)
	lecl_tables_ptr->S_char_to_simple_class [i] = classe [i];

    lecl_tables_ptr->SXS_adrp = (struct S_adrp_s*) sxalloc (last_static_ste + 1, sizeof (struct S_adrp_s));
    lecl_tables_ptr->SXS_adrp [0].xterlis = 0;
    lecl_tables_ptr->SXS_adrp [0].syno_lnk = 0;
    lecl_tables_ptr->SXS_adrp [0].lgth = 0;
    lecl_tables_ptr->S_tbl_size.S_terlis_lgth = 0;
    max_t_code_name_lgth = 0;

    for (i = 1; i < termax; i++) {
	lecl_tables_ptr->SXS_adrp [i].xterlis = lecl_tables_ptr->S_tbl_size.S_terlis_lgth;
	lecl_tables_ptr->SXS_adrp [i].syno_lnk = 0;
	lecl_tables_ptr->SXS_adrp [i].lgth = sxstrlen (t_to_ate [i]) - 2;
	lecl_tables_ptr->S_tbl_size.S_terlis_lgth += lecl_tables_ptr->SXS_adrp [i].lgth;
	max_t_code_name_lgth = (max_t_code_name_lgth > lecl_tables_ptr->SXS_adrp [i].lgth) ? max_t_code_name_lgth :
	     lecl_tables_ptr->SXS_adrp [i].lgth;
    }

    lecl_tables_ptr->SXS_adrp [termax].xterlis = lecl_tables_ptr->S_tbl_size.S_terlis_lgth;
    lecl_tables_ptr->SXS_adrp [termax].syno_lnk = 0;
    lecl_tables_ptr->SXS_adrp [termax].lgth = 11;
    lecl_tables_ptr->S_tbl_size.S_terlis_lgth += 11;
    max_t_code_name_lgth = (max_t_code_name_lgth > 11) ? max_t_code_name_lgth : 11;

    x = termax + 1;

    for (i=1; i <= nbndef;i++) {
	SXINT t, l;

	if ((t = keywords [i].t_code) < 0) {
	    /* synonym */
	    t = -t;
	    l = sxstrlen (keywords [i].string_table_entry) - 2;
	    lecl_tables_ptr->SXS_adrp [x].xterlis = lecl_tables_ptr->S_tbl_size.S_terlis_lgth;
	    max_t_code_name_lgth = (max_t_code_name_lgth > l) ? max_t_code_name_lgth : l;
	    lecl_tables_ptr->S_tbl_size.S_terlis_lgth += l;
	    lecl_tables_ptr->SXS_adrp [x].syno_lnk = lecl_tables_ptr->SXS_adrp [t].syno_lnk;
	    lecl_tables_ptr->SXS_adrp [t].syno_lnk = x;
	    lecl_tables_ptr->SXS_adrp [x].lgth = l;
	    x++;
	}
    }

    lecl_tables_ptr->S_terlis = (char*) sxalloc (lecl_tables_ptr->S_tbl_size.S_terlis_lgth + 1, sizeof (char));

    {
	char	*s, *head, *tail;

	s = lecl_tables_ptr->S_terlis;

	for (i = 1; i < termax; i++)
	    for (head = sxstrget (t_to_ate [i]) + 1, tail = head + lecl_tables_ptr->SXS_adrp [i].lgth; head < tail; )
		*s++ = *head++;

	*s = NUL;
	strcat (s, "End Of File");
	s += 11;

	for (i=1; i <= nbndef;i++) {
	    if (keywords [i].t_code < 0) {
		/* synonym */
		SXINT ate;

		ate = keywords [i].string_table_entry;

		for (head = sxstrget (ate) + 1, tail = head + sxstrlen (ate) - 2; head < tail; )
		    *s++ = *head++;
	    }
	}

	*s = NUL;
    }

    lecl_tables_ptr->S_transition_matrix = (transition_matrix_s**) transition_matrix;
    transition_matrix = NULL;
    lecl_tables_ptr->SXS_action_or_prdct_code = (struct action_or_prdct_code_s*) action_or_prdct_code;
    action_or_prdct_code = NULL;
    lecl_tables_ptr->S_is_a_generic_terminal = is_a_generic_terminal;
    is_a_generic_terminal = NULL;
    lecl_tables_ptr->S_is_a_keyword = is_a_keyword;
    is_a_keyword = NULL;

    if (!lecl_write (lecl_tables_ptr, prgentname))
	SXEXIT (2);


/* fin de la construction/allocation/sortie des tables du scanner */

    if (check_kw != NULL) {
	varstr_free (check_kw);
	lecl_tables_ptr->S_check_kw = NULL;
    }

    lecl_free (lecl_tables_ptr); /* end lecl_tbls_out */
}
