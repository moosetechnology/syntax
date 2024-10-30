/*   S X I N D P A R S E R . H   */

#include "sxunix.h"
#include "B_tables.h"
#include "sxba.h"
#include "XxY.h"
#include "XH.h"
#include "SS.h"

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x], T [x] = 0

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)

#define XT_TO_T_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? xt : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].v_code)
#define XT_TO_PRDCT_CODE(xt) ((xt+bnf_ag.WS_TBL_SIZE.tmax<=0) ? -1 : bnf_ag.XT_TO_T_PRDCT[xt+bnf_ag.WS_TBL_SIZE.tmax].prdct_no)
#define XNT_TO_NT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? xnt : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].v_code)
#define XNT_TO_PRDCT_CODE(xnt) ((xnt-bnf_ag.WS_TBL_SIZE.ntmax<=0) ? -1 : bnf_ag.XNT_TO_NT_PRDCT[xnt-bnf_ag.WS_TBL_SIZE.ntmax].prdct_no)

#define get_t_string(t)  (bnf_ag.T_STRING + bnf_ag.ADR [t])
#define get_nt_string(nt)  (bnf_ag.NT_STRING + bnf_ag.ADR [nt])
#define get_nt_length(nt) (bnf_ag.ADR [nt+1] - bnf_ag.ADR [nt] - 1)

#define grand 100000

#define Q0_to_tnt_trans(s)	(Q0 [s].in)

#define XH_BOT(h,x)		((h).list + (h).display [x].X)
#define XH_TOP(h,x)		((h).list + (h).display [(x)+1].X)

#define Q0_V_to_items		Q0_V_to_next

struct bnf_ag_item	bnf_ag;

SXINT			lr0_automaton_state_nb,
                        reduce_item_no,
                        max_red_per_state,
                        xac2;

SXBA			*LR0_sets,
                        Next_states_set,
                        *FIRST;

FILE			*statistics_file;


XH_header		nucleus_hd;
XxY_header		Q0xV_hd;
SXINT		 	*Q0xV_to_Q0;
XxY_header		Q0xQ0_hd;


struct Q0 {
    SXINT		in, xla, bot, t_trans_nb, nt_trans_nb, red_trans_nb;
};


struct Q0		*Q0;

/*   E N D    S X I N D P A R S E R . H   */
