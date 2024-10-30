/*   R L R _ O P T I M . H  */

#include "sxunix.h"
#include "put_edit.h"
#include "varstr.h"
#include "B_tables.h"
#include "sxba.h"
#include "XxYxZ.h"
#include "XH.h"
#include "SS.h"

#define min(x,y) (((x)<=(y))?(x):(y))
#define max(x,y) (((x)>=(y))?(x):(y))

#define NonTerminal	0
#define Terminal	1
#define Predicate	2

#define Error		0
#define ScanShift	1
#define Shift		2
#define ScanReduce	3
#define Reduce		4
#define Prdct		5
#define ForceError	6
#define LaScanShift	7
#define NonDeter	8

#define NO_CONFLICT_	000000
#define NOT_LALRk_	000001
#define NOT_LALR_	000003
#define NOT_LRk_	000005
#define NOT_LR_		000017
#define NOT_RhLALRk_	000020
#define NOT_RoxoLALRk_	000040
#define NOT_RhLALR0_	000100
#define NOT_RoxoLALR0_	000200
#define NOT_RLALR_	000763
#define NOT_RoxoLRk_	001040
#define NOT_RoxoLR0_	002200
#define NOT_LRPI_	007777
#define AMBIGUOUS_	017777
#define ALL_CONFLICT_	037777

#define FSA_		0
#define UNBOUNDED_	1
#define BOUNDED_	2

#define PUSH(T,x)	T [x] = T [0], T [0] = x
#define POP(T,x)	x = T [0], T [0] = T [x], T [x] = 0

#define	LGPROD(r)	(bnf_ag.WS_NBPRO [r + 1].prolon - bnf_ag.WS_NBPRO [r].prolon - 1)
#define PROD_TO_RED(x)	(x > bnf_ag.WS_TBL_SIZE.actpro ? bnf_ag.RULE_TO_REDUCE [x - bnf_ag.WS_TBL_SIZE.actpro] : x)

#define HASH_SIZE 257

#if 0
extern char* get_xnt_string();
#endif

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
#define XQ0_TO_Q0(s)		((clone_to_lr0_state == NULL) || (s <= lr0_automaton_state_nb) ? s : clone_to_lr0_state [s - lr0_automaton_state_nb])
int		*clone_to_lr0_state, lr0_automaton_state_nb;


char	constructor_name [24];

struct bnf_ag_item	bnf_ag;

bool is_automaton,
	is_list_user_conflicts,
	is_list_system_conflicts,
	is_list_conflicts,
	is_abstract,
	is_floyd_evans,
	is_long_listing,
	is_force,
        is_pspe,
        is_tspe,
        is_nspe,
        is_unique_conflict_per_state,
        is_unique_derivation_per_conflict,
        is_lr_constructor,
        is_rlr_constructor,
        is_ambiguity_check,
        is_statistics,
        is_listing_opened,
	sxverbosep,
        is_ambiguous,
        is_super_arc,
        is_not_LRpi,
        is_non_deterministic_automaton,
        has_xprod,
        print_time,
	sxttycol1p;

int	conflict_derivation_kind,
        reduce_item_no,
        max_red_per_state;
SXINT   xac2;
int     xe1,
        xpredicates,
	xt_items,
        xnt_items,
        xprdct_items,
	any,
        t_error,
        force_error,
	t_vector_size,
        nt_vector_size,
        nt_fe_size,
	MIN,
        MAX,
        delta,
        t_MIN,
        t_delta,
        init_state,
        final_state,
        NTMAX,
        MAX_LA,
        h_value,
        k_value,
        k_value_max;

VARSTR  vstr;


SXBA	single_prod /* xnbpro */,
	s_red_set /* xnbpro */,
        not_used_red_set /* xnbpro */,
	t_check_set,
        xnt_state_set,
        xt_state_set,
        nd_state_set,
        *LR0_sets,
        Next_states_set,
	*FIRST;

FILE	*listing, *statistics_file;


XH_header		nucleus_hd;
XxY_header		Q0xV_hd;
int		 	*Q0xV_to_Q0;
XxY_header		Q0xQ0_hd;
XxY_header		includes_hd;
X_header		StNt_reads_cycle_hd;
int			*StNt_to_reads_cycle_nb, Q0xQ0_top;
XxY_header		ref_to_ee;


struct Q0 {
    int		in, xla, bot, t_trans_nb, nt_trans_nb, red_trans_nb;
};


struct state {
    int		lgth,
                start;
    };

struct floyd_evans {
    int		codop,
                reduce,
                pspl,
                next;
    };

struct stat_vars {
    int		lines,
                cols;
    };

struct stat_tbl {
    struct stat_vars	i,
                        r,
                        c;
    };

struct statistics {
    struct stat_tbl	t,
                        nt;
    };

struct Q0	*Q0;

SXBA	nullable_set;

struct statistics	stats;

struct state	*t_states,
                *nt_states,
                *predicates;

struct P_item	*t_items,
                *nt_items;

struct floyd_evans	*t_fe,
	                *nt_fe,
	                *tnt_fe;

struct P_reduction	*reductions;

struct P_base	*t_bases;

struct P_base	*nt_bases;

struct P_item	*vector;

struct P_prdct	*prdcts;

struct P_prdct_list	*prdct_list;

int	Mtstate,
    	Mntstate,
    	Mvec,
    	deltavec,
    	mMprdct,
    	mMred,
    	mMrednt,
    	Mprdct,
    	Mfe,
    	M0ref,
    	Mref,
    	Mred,
    	mMfe,
        mgerme,
        Mgerme,
        nd_degree,
        prdct_list_top;


int	*t_state_equiv_class,
        *nt_state_equiv_class,
	*t_lnks,
	*nt_lnks,
	*tnt_lnks,
	*prdct_lnks,
	*nt_to_ad,
        *nt_to_nt,
        *germe_to_gr_act /* Fait correspondre a chaque ARC (germe) une instruction reduce
			   du conflit */;

int	t_hash [HASH_SIZE],
        nt_hash [HASH_SIZE],
        tnt_hash [HASH_SIZE],
        prdct_hash [HASH_SIZE];



extern int	LALR1 (void), OPTIM (void);
extern void	sat_mess (char *name, int no, int old_size, int new_size);

/*   E N D    R L R _ O P T I M . H  */
