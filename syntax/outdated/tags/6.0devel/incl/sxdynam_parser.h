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
#ifndef sxdynam_parser_h
#define sxdynam_parser_h

# include "sxunix.h"
# include "SS.h"
# include "X.h"
# include "XxY.h"
# include "XxYxZ.h"
# include "sxlist.h"

extern SXBOOLEAN	sxdp_write (sxfiledesc_t file_descr /* file descriptor */);
extern SXBOOLEAN	sxdp_read (sxfiledesc_t file_descr /* file descriptor */);
extern SXVOID	sxdp_alloc (SXINT rule_nb);
extern SXVOID	sxdp_free (void);
extern SXINT	sxdp_rule_is_set (SXINT rhs_lgth, SXINT *symbols);
extern SXBOOLEAN	sxdp_rule_is_activated (SXINT rule);
extern SXBOOLEAN	sxdp_rule_set (SXINT rhs_lgth, SXINT *symbols, SXINT *rule, SXINT action_nb, SXINT prio, SXINT assoc);
extern SXVOID	sxdp_rule_get_attr (SXINT rule, SXINT *action_nb, SXINT *prio, SXINT *assoc);
extern SXVOID	sxdp_rule_set_attr (SXINT rule, SXINT action_nb, SXINT prio, SXINT assoc);
extern SXVOID	sxdp_rule_activate (SXINT rule);
extern SXVOID	sxdp_rule_deactivate (SXINT rule);
extern SXVOID	sxdp_rule_delete (SXINT rule);
extern SXVOID	sxdp_init_state (void);
extern SXINT	sxdynam_parser (SXINT what_to_do, struct sxtables *arg);
extern SXBOOLEAN	sxdp_new_xt (SXINT t, SXINT prdct, SXINT *xt, SXINT prio, SXINT assoc);
extern SXBOOLEAN	sxdp_new_xnt (SXINT nt, SXINT prdct, SXINT *xnt);
extern SXVOID	sxdp_delete_xt (SXINT xt);
extern SXVOID	sxdp_delete_xnt (SXINT xnt);
extern SXBOOLEAN	sxdp_add_new_state (SXINT transition, SXINT *result);

struct priority {
    SXINT assoc, value;
};

struct ambig_stack_elem {
    SXINT rule;
};

struct pstack_to_attr {
    SXINT rule_or_trans, son, brother, la_lgth;
};

#define UNDEF_ASSOC		0
#define LEFT_ASSOC		1
#define RIGHT_ASSOC		2
#define NON_ASSOC		3


struct sxdynam_grammar {
    sxlist_header
	        states;

    XxY_header	P,		/* rule = (lhs, rhs)			*/
                rhs,		/* rhs = (symbol, rhs)			*/
                items,		/* item = (rule, rhs)			*/
                transitions,	/* transition = (state, symbol)		*/
                item_occurs,	/* item_occur = (transition, item)	*/
                transitionxnext_state,
                                /*         = (transition, state)	*/
                xt,
                xnt;

    SXBA	item_set,
                trans_has_prdct,
                is_state_linked,
                active_rule_set;

    SXINT		*rule_to_rhs_lgth,
	        *rule_to_first_item,
	        *rule_to_action,
	        *state_to_action,
                *state_to_in_symbol,
                *state_to_used_lnk,
	        *item_to_next_item,
	        *trans_to_next_state,
                *prdct_stack,
                *next_state_stack,
                *item_stack;

    SXINT		lgt1,
                init_state,
                init_transition,
                super_start_symbol,
                start_symbol,
                super_rule,
                eof_code,
                parse_stack,
                Mtok_no,
                ambig_stack_size,
                ambig_stack_top;

    struct priority
	        *t_priorities,
	        *r_priorities;

    XxYxZ_header
	        parse_stacks;	/* pstack = (pstack, state, ptok_no)	*/

    struct pstack_to_attr
	        *pstack_to_attr;

    struct sxtoken
	        *(*token_filter) (SXINT);

    struct ambig_stack_elem
	        *ambig_stack;

    SXINT		(*desambig) (void);

    SXBOOLEAN	is_in_look_ahead,
                has_been_updated,
                has_been_erased;
};

#ifndef SX_GLOBAL_VAR_SXDG
#define SX_GLOBAL_VAR_SXDG extern
#endif

SX_GLOBAL_VAR_SXDG struct sxdynam_grammar SXDG;


#define XT_TO_T(G,x)		XxY_X ((G).xt, -x)
#define XT_TO_PRDCT(G,x)	XxY_Y ((G).xt, -x)

#define XNT_TO_NT(G,x)		XxY_X ((G).xnt, x)
#define XNT_TO_PRDCT(G,x)	XxY_Y ((G).xnt, x)

#endif
