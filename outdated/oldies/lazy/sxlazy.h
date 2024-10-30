# include "sxunix.h"
# include "sxba.h"
# include "SS.h"
# include "XxY.h"
# include "XxYxZ.h"

extern bool	sxl_add_rule ();
extern void	sxl_clear_rule_from_automaton ();
extern void	sxl_clear_rule ();
extern void	sxl_add_rule_to_automaton ();
extern void	sxl_init_state ();
extern bool	sxlazy ();
extern int	sxl_new_xt ();
extern int	sxl_new_nt ();


struct symbolxprdct {
    int symbol, prdct;
};

struct priority {
    int assoc, value;
};

struct ambig_stack_elem {
    int rule;
};

struct config_to_attr {
    int rule_or_trans, son, brother, la_lgth;
};

#define UNDEF_ASSOC		0
#define LEFT_ASSOC		1
#define RIGHT_ASSOC		2
#define NON_ASSOC		3


struct grammar {
    XxY_header	P,		/* rule = (lhs, rhs)			*/
                rhs,		/* rhs = (symbol, rhs)			*/
                items,		/* item = (rule, rhs)			*/
                transitions,	/* transition = (state, symbol)		*/
                item_occurs,	/* item_occur = (transition, item)	*/
                nucleus,	/* nucleus = (nucleus, item)		*/
                transitionxnext_state,
                                /*         = (transition, state)	*/
                statexitem_hd,	/* 	   = (state, item)		*/
                tXste;

    XxYxZ_header
	        configurations;	/* config = (config, state, ptok_no)	*/

    sxindex_header
	        states_tank,
	        xt_tank,
	        xnt_tank,
	        prdct_tank;

    SXBA	item_set,
                nucleus_set,
                config_set,
                active_rule_set;

    int		*rule_to_rhs_lgth,
	        *rule_to_first_item,
	        *rule_to_action,
	        *state_to_action,
	        *state_to_nucleus,
                *state_to_in_symbol,
	        *item_to_next_item,
	        *nucleus_to_state,
	        *trans_to_next_state,
                *prdct_stack,
                *next_state_stack,
                *tXste_to_xt;

    struct symbolxprdct
	        *xnt_to_ntxprdct,
	        *xt_to_txprdct;

    int		error_state,
                lgt1,
                empty_nucleus,
                init_state,
                super_start_symbol,
                start_symbol,
                super_rule,
                eof_code,
                configuration,
                config_set_size,
                Mtok_no,
                ambig_stack_size,
                ambig_stack_top;

    struct ambig_stack_elem
	        *ambig_stack;

    struct priority
	        *t_priorities,
	        *r_priorities;

    struct config_to_attr
	        *config_to_attr;

    struct sxtoken
	        *(*token_filter) ();

    int		(*desambig) ();

    bool	is_in_look_ahead;
};


struct grammar G;


#define XT_TO_T(xt)		G.xt_to_txprdct [-xt].symbol
#define XT_TO_PRDCT(xt)		G.xt_to_txprdct [-xt].prdct

#define XNT_TO_NT(xnt)		G.xnt_to_ntxprdct [xnt].symbol
#define XNT_TO_PRDCT(xnt)	G.xnt_to_ntxprdct [xnt].prdct
