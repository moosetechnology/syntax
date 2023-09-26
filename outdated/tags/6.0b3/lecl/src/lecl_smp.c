/*   S E M A N T I C _ P A S S   */

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

/************************************************************************/
/* Historique des modifications, en ordre chronologique inverse:	*/
/************************************************************************/
/* 20030516 13:07 (phd):	Suppression de #ifdef MSDOS		*/
/************************************************************************/
/* 20000906 15:21 (phd):	Adaptation à MINGW			*/
/************************************************************************/
/* 20000906 15:20 (phd):	Ajout de cette rubrique "modifications"	*/
/************************************************************************/

/*****************************************************************/
/*                                                               */
/*                                                               */
/*  This program has been translated from lecl.semantic_pass.pl1 */
/*  on Monday the tenth of March, 1986, at 12:17:02,             */
/*  on the Multics system at INRIA,                              */
/*  by the release 3.3g PL1_C translator of INRIA,               */
/*         developped by the "Langages et Traducteurs" team,     */
/*         using the SYNTAX (*), FNC and Paradis systems.        */
/*                                                               */
/*                                                               */
/*****************************************************************/
/* (*) SYNTAX is a trademark of INRIA.                           */
/*****************************************************************/


#define NODE struct lecl_node

#include "sxunix.h"
#include <ctype.h>
#include "B_tables.h"
#include "varstr.h"
#include "lecl_ag.h"

#include "lecl_node.h"

#include "lecl_pcn.h"

#include "S_tables.h" /* for struct lecl_tables_s */

char WHAT_LECLSMP[] = "@(#)SYNTAX - $Id: lecl_smp.c 1114 2008-02-27 15:13:51Z rlacroix $" WHAT_DEBUG;





extern SXINT	lecl_sature (SXINT nbt);
extern VOID	lecl_css (struct lecl_node *visited);
extern VOID	lecl_st (void);
extern VOID	lecl_fsa_construction (void);
extern VOID	lecl_code_gen (void);
extern VOID	gen_check_kw (void);
extern VOID	lecl_tbls_out (struct lecl_tables_s *lecl_tables_ptr);
extern VOID	lecl_list_out (void);
extern VOID	lecl_ag_free (void);

static long	bnf_modif_time;






static VOID	smpopen (struct sxtables *sxtables_ptr)
{
    err_titles = sxtables_ptr->err_titles;
    sxatcvar.atc_lv.node_size = sizeof (struct lecl_node);
}




/* The storage is allocated here ! */

static VOID smppass (void)
{
    char	*s, *s1;
    SXINT		c;

    substate = NULL;
    fsa = NULL;
    item = NULL;
    fsa_trans = NULL;
    abbrev = NULL;
    abbrev_or_class_to_ate = NULL;
    action_or_prdct_to_ate = NULL;
    prdct_to_ers = NULL;
    not_declared = NULL;
    ers = NULL;
    ers_disp = NULL;
    keywords = NULL;
    exclude = NULL;
    keyword = NULL;
    t_to_ate = NULL;
    ate_to_t = NULL;
    classe = NULL;
    target_code = NULL;
    target_collate = NULL;
    simple_classe = NULL;
    action_or_prdct_code = NULL;
    synonym_list = NULL;
    synonym = NULL;
    ate_to_token_no = NULL;
    esc_to_sc_prdct = NULL;
    compound_classes = NULL;
    kod = NULL;
    class_to_char_set = NULL;
    suivant = NULL;
    RE_to_T = NULL;
    sc_to_char_set = NULL;
    new_sc_to_char_set = NULL;
    tfirst = NULL;
    tnext = NULL;
    follow = NULL;
    ctxt = NULL;
    class_set = NULL;
    is_re_generated = NULL;
    is_class_used_in_re = NULL;
    is_a_keyword = NULL;
    t_is_defined_by_a_token = NULL;
    is_a_generic_terminal = NULL;

    if (sxatcvar.atc_lv.abstract_tree_is_error_node)
	goto no_automaton;


/* initialisations par defaut */

    lgt [1] = 50;
    /* taille max de substate */
    lgt [2] = 100;
    /* nombre max d'etats */
    lgt [3] = 500;
    /* taille max de item */
    lgt [4] = 250;


/* taille max de fsa_trans */

/* LECTURE DES TABLES DE BNF */

    {
	struct bnf_ag_item	bnf_ag;
	SXINT	i, lgth, code;
	long	ate;
	char	terlis [256];

	if (!bnf_read (&bnf_ag, prgentname))
	    goto no_automaton;

	bnf_modif_time = bnf_ag.bnf_modif_time;
	termax = -bnf_ag.WS_TBL_SIZE.tmax;
	is_a_generic_terminal = sxba_calloc (termax + 1);
	sxba_copy (is_a_generic_terminal, bnf_ag.IS_A_GENERIC_TERMINAL);
	follow = sxbm_calloc (termax + 1, termax + 1);

	for (i = 1; i <= termax; i++) {
	    sxba_copy (follow [i], bnf_ag.FOLLOW [i]);
	}

	true_ate = sxstrsave ("&True");
	false_ate = sxstrsave ("&False");
	/*  A L L O C A T I O N    O F   S Y M B O L   T A B L E   I T E M S   */
	lecl_sature (0);
	lecl_css (sxatcvar.atc_lv.abstract_tree_root);
	ate_to_t_top = ate_to_token_no_top = ((SXINT) sxstrmngr.top) + 2 * termax + synonym_no + predef_class_name_no + 1
	     /* Include eventuel */ ;
	char_max = 256 /* Le 9/14/99 prudence */ /*++char_max < 128 ? 128 : char_max */;
	class_size += char_max + predef_class_name_no;
	action_size -= xactmin;
	token_size += termax + synonym_no + 1;
	abbrev_size++;
	predicate_size += 2 /* prdct_true_code, prdct_false_code */ ;
	target_collate_length = 1 << byte_length;
	ate_to_token_no = (SXINT*) sxcalloc ((SXINT) ate_to_token_no_top + 1, sizeof (SXINT));
	t_to_ate = (SXINT*) sxalloc ((SXINT) termax + 1, sizeof (SXINT));
	ate_to_t = (SXINT*) sxalloc ((SXINT) ate_to_t_top + 1, sizeof (SXINT));
	abbrev = (struct abbrev_item*) sxalloc ((SXINT) abbrev_size, sizeof (struct abbrev_item));
	abbrev += abbrev_size;
	abbrev_or_class_to_ate = (SXINT*) sxalloc ((SXINT) abbrev_size + (SXINT) class_size + 1, sizeof (SXINT));
	abbrev_or_class_to_ate += abbrev_size;
	action_or_prdct_to_ate = (SXINT*) sxalloc ((SXINT) action_size + (SXINT) predicate_size + 1, sizeof (SXINT));
	action_or_prdct_to_ate += action_size;
	prdct_to_ers = (SXINT*) sxalloc ((SXINT) predicate_size + 1, sizeof (SXINT));
	class_to_char_set = sxbm_calloc (class_size + 1, char_max + 1);
	is_class_used_in_re = sxba_calloc (class_size + 1);
	not_declared = (SXINT*) sxalloc ((SXINT) (class_size + 1), sizeof (SXINT));
	ers_disp = (struct ers_disp_item*) sxcalloc ((SXINT) token_size + 1, sizeof (struct ers_disp_item));
	RE_to_T = sxbm_calloc (token_size + 1, termax + 1);
	keywords = (struct keywords_item*) sxalloc ((SXINT) token_size + 1, sizeof (struct keywords_item));
	exclude = (struct exclude_item*) sxalloc ((SXINT) exclude_size + 1, sizeof (struct exclude_item));
	keyword = (SXINT*) sxalloc ((SXINT) keyword_size + 1, sizeof (SXINT));
	is_a_keyword = sxba_calloc (termax + 1);
	t_is_defined_by_a_token = sxba_calloc (termax + 1);
	classe = (SXINT*) sxalloc ((SXINT) target_collate_length, sizeof (SXINT));
	simple_classe = (SXINT*) sxalloc ((SXINT) char_max + 1, sizeof (SXINT));
	target_code = (SXINT*) sxalloc ((SXINT) target_collate_length, sizeof (SXINT));
	target_collate = (SXINT*) sxalloc ((SXINT) target_collate_length, sizeof (SXINT));


/* Remplissage de t_to_ate et ate_to_t */

	for (i = 0; i <= ate_to_t_top; i++) {
	    ate_to_t [i] = -2;
	}


/* aucune confusion possible avec les terminaux du niveau syntaxique */

	ate = sxstrsave ("Comments");
	t_to_ate [0] = ate;
	ate_to_t [ate] = 0;
	terlis_lgth /* Sert a calculer la taille cumulee des E.R. */  = 2
	/* EOF par defaut */
									 ;
	max_re_lgth = 1 /* EOF */ ;

	for (i = 1; i < termax; i++) {
	    /* pour convertir les indices de chaines PL/1 contenus dans "intro_adr" en
   indices de chaines C pour acceder a "t_string" */
	    lgth = bnf_ag.ADR [-i - 1] - bnf_ag.ADR [-i] - 1;
	    max_re_lgth = max_re_lgth > lgth ? max_re_lgth : lgth;
	    terlis [0] = '"';

/*	    for (s = terlis + 1, s1 = bnf_ag.T_STRING + bnf_ag.ADR [-i]; c = *s1; s++, s1++, terlis_lgth++)
		*s = c; */

	    for (s = terlis + 1, s1 = bnf_ag.T_STRING + bnf_ag.ADR [-i]; *s1; s++, s1++, terlis_lgth++)
		*s = *s1;

	    *s++ = '"';
	    *s = '\0';
	    ate = sxstrsave (terlis);
	    t_to_ate [i] = ate;
	    ate_to_t [ate] = i;


/* Pour pouvoir reconnaitre des denotations de terminaux du niveau syntaxique
   (en partie gauche de tokens) qui ont ete capitalises par le scanner de lecl
*/

/*	    for (s = terlis + 1; c = *s; s++)
		if (islower (c))
		    *s = toupper (c)*/ /* passage en majuscules */ ;

	    for (s = terlis + 1; (c = *s); s++)
		*s = sxtoupper (c) /* passage en majuscules, nouvelle def de toupper */ ;

	    *--s = '\0';
	    ate = sxstrsave (terlis + 1);
	    code = ate_to_t [ate];

	    if (code == -2 /* 1ere definition */ )
		ate_to_t [ate] = i;
	    else
		ate_to_t [ate] = -1; /* collision sur les noms capitalises */
	}


/* fin du for (i... */

       /* aucune confusion avec les terminaux du niveau syntaxique */

	ate = sxstrsave ("Eof");
	t_to_ate [i] = ate;
	code = ate_to_t [ate];

	if (code == -2 /* 1ere definition */ )
	    ate_to_t [ate] = i;
	else
	    /* collision */
	    ate_to_t [ate] = -1;

	bnf_free (&bnf_ag); /* fin de la lecture des tables de bnf */
    }

    lecl_st ();
    st_done = TRUE;

    if (is_check) {
	if (is_source)
	    lecl_list_out ();

	lecl_ag_free ();
	return;
    }

    if (sxerrmngr.nbmess [2] > 0)
	goto no_automaton;

    lecl_fsa_construction ();

    if (sxerrmngr.nbmess [2] > 0)
	goto no_automaton;

    fsa_done = TRUE;
    lecl_code_gen ();
    gen_check_kw ();

    if (is_source)
	lecl_list_out ();

    {


	struct lecl_tables_s	lecl_tables;

	lecl_tables.bnf_modif_time = bnf_modif_time;
	lecl_tbls_out (&lecl_tables);
    }

    lecl_ag_free ();
    return;

no_automaton:
    sxtmsg (sxsrcmngr.source_coord.file_name, "%sConstruction failed : the finite state automaton is not generated.",
	 err_titles [2]);

    if (is_source)
	lecl_list_out ();

    lecl_ag_free ();
}




VOID	lecl_smp (SXINT what, struct sxtables *sxtables_ptr)
{
    switch (what) {
    case OPEN:
	smpopen (sxtables_ptr);
	break;

    case ACTION:
	if (sxverbosep) {
	    if (!sxttycol1p) {
		fputc ('\n', sxtty);
		sxttycol1p = TRUE;
	    }

	    fputs ("Semantic Pass\n", sxtty);
	}

	smppass ();
	break;

    case CLOSE:
	break;

    default:
	fputs ("The function \"lecl_smp\" is out of date with respect to its specification.\n", sxstderr);
	sxexit(1);
    }
}
