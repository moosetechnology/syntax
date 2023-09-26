#ifndef sxdynam_parser_h
#define sxdynam_parser_h

# include "sxunix.h"
# include "SS.h"
# include "X.h"
# include "XxY.h"
# include "XxYxZ.h"
# include "sxlist.h"

extern BOOLEAN	sxdp_write ();
extern BOOLEAN	sxdp_read ();
extern SXVOID	sxdp_alloc ();
extern SXVOID	sxdp_free ();
extern int	sxdp_rule_is_set ();
extern BOOLEAN	sxdp_rule_is_activated ();
extern BOOLEAN	sxdp_rule_set ();
extern SXVOID	sxdp_rule_get_attr ();
extern SXVOID	sxdp_rule_set_attr ();
extern SXVOID	sxdp_rule_activate ();
extern SXVOID	sxdp_rule_deactivate ();
extern SXVOID	sxdp_rule_delete ();
extern SXVOID	sxdp_init_state ();
extern int	sxdynam_parser ();
extern BOOLEAN	sxdp_new_xt ();
extern BOOLEAN	sxdp_new_nt ();
extern SXVOID	sxdp_delete_xt ();
extern SXVOID	sxdp_delete_xnt ();
extern BOOLEAN	sxdp_add_new_state ();

struct priority {
    int assoc, value;
};

struct ambig_stack_elem {
    int rule;
};

struct pstack_to_attr {
    int rule_or_trans, son, brother, la_lgth;
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

    int		*rule_to_rhs_lgth,
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

    int		lgt1,
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
	        *(*token_filter) ();

    struct ambig_stack_elem
	        *ambig_stack;

    int		(*desambig) ();

    BOOLEAN	is_in_look_ahead,
                has_been_updated,
                has_been_erased;
};


struct sxdynam_grammar SXDG;


#define XT_TO_T(G,x)		XxY_X ((G).xt, -x)
#define XT_TO_PRDCT(G,x)	XxY_Y ((G).xt, -x)

#define XNT_TO_NT(G,x)		XxY_X ((G).xnt, x)
#define XNT_TO_PRDCT(G,x)	XxY_Y ((G).xnt, x)

#endif
